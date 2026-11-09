#include "L1TriggerScouting/TokenTime/plugins/alpaka/TransformKernel.h"

#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "DataFormats/L1ScoutingSoA/interface/TokenHostTensor.h"

#include "PerfTools/Perfetto/interface/CMSSWPerfettoTrace.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::l1sc::kernels {

  using namespace cms::alpakatools;

  template <typename TAcc, typename T>
  ALPAKA_FN_ACC T lp(const TAcc& acc, T pt) {
    return alpaka::math::log(acc, pt) - T(1.8);
  }

  template <typename TAcc, typename T>
  ALPAKA_FN_ACC T cosphi(const TAcc& acc, T phi) {
    return alpaka::math::cos(acc, phi);
  }

  template <typename TAcc, typename T>
  ALPAKA_FN_ACC T sinphi(const TAcc& acc, T phi) {
    return alpaka::math::sin(acc, phi);
  }

  // pdgid -> tokenizer PID class (neutral_hadron, photon, h-, h+, e-, e+, mu-, mu+)
  ALPAKA_FN_HOST_ACC inline int pidClass(int16_t pdgid) {
    switch (pdgid) {
      case 22:
        return 1;
      case -211:
        return 2;
      case 211:
        return 3;
      case 11:
        return 4;
      case -11:
        return 5;
      case 13:
        return 6;
      case -13:
        return 7;
      default:
        return 0;  // 130 and anything else: neutral hadron
    }
  }

  // ------------------------------------------------------------------------------------------------------------------------
  // --------------------------- fill the token input tensor: one block per bx, one thread per candidate --------------------
  class ComputePuppiFeaturesKernel {
  public:
    ALPAKA_FN_ACC void operator()(Acc1D const& acc,
                                  PuppiDeviceCollection::ConstView puppi,
                                  BxLookupDevice::ConstView bx_lookup,
                                  TokenInputDeviceTensor::View input_tensors) const {
      const uint32_t nbx = bx_lookup.offset().metadata().size() - 1;
      for (auto bx_idx : independent_groups(acc, nbx)) {
        // candidate range of this bx
        const uint32_t begin = bx_lookup.offset()[bx_idx].offset();
        const uint32_t end = bx_lookup.offset()[bx_idx + 1].offset();
        const uint32_t bx_size = end - begin;
        if (bx_size == 0)
          continue;

        // the model takes the first 128 candidates of each bx; padding rows stay zero (zeroInitialise)
        constexpr uint32_t kMaxRows = PuppiFeatures::RowsAtCompileTime;
        const uint32_t input_size = (bx_size > kMaxRows) ? kMaxRows : bx_size;

        auto input_tensor = input_tensors[bx_idx];
        for (auto tid : independent_group_elements(acc, input_size)) {
          const auto p = begin + tid;  // global candidate index
          // order must match training: [eta/3, cos(phi), sin(phi), log(pt) - 1.8]
          input_tensor.features()(tid, 0) = puppi.eta()[p] / 3.0f;
          input_tensor.features()(tid, 1) = cosphi(acc, puppi.phi()[p]);
          input_tensor.features()(tid, 2) = sinphi(acc, puppi.phi()[p]);
          input_tensor.features()(tid, 3) = lp(acc, puppi.pt()[p]);
          // PID class, same order as the unpacker pdgid table and the training classes:
          // {130, 22, -211, 211, 11, -11, 13, -13} -> 0..7
          input_tensor.features()(tid, 4) = static_cast<float>(pidClass(puppi.pdgid()[p]));
          // mask: 1 = real candidate (padding rows stay 0 from zeroInitialise)
          input_tensor.features()(tid, 5) = 1.0f;
        }
      }
    }
  };
  // ------------------------------------------------------------------------------------------------------------------------

  TokenInputDeviceTensor transform(Queue& queue, const PuppiDeviceCollection& puppi, const BxLookupDevice& bx_lookup) {
    // one input matrix per bx
    const auto nbx = bx_lookup.const_view().offset().metadata().size() - 1;
    auto input_tensor = TokenInputDeviceTensor(queue, nbx);
    input_tensor.zeroInitialise(queue);
    if (nbx == 0)
      return input_tensor;

    // one block per bx, one thread per row of the 128-row matrix
    const auto threadsPerBlock = PuppiFeatures::RowsAtCompileTime;
    const auto workDiv = make_workdiv<Acc1D>(nbx, threadsPerBlock);

    {
      CMS_PERFETTO_SCOPE("ComputePuppiFeaturesKernel");
      alpaka::exec<Acc1D>(
          queue, workDiv, ComputePuppiFeaturesKernel{}, puppi.const_view(), bx_lookup.const_view(), input_tensor.view());
    }

    /* BEGIN DEBUG */
    // auto host_tensor = TokenInputHostTensor(cms::alpakatools::host(), nbx);
    // alpaka::memcpy(queue, host_tensor.buffer(), input_tensor.const_buffer());
    // alpaka::wait(queue);
    //
    // std::ofstream out_stream("token_input_tensor.csv", std::ios::out);
    // out_stream << "bx_idx,row,eta_3,cosphi,sinphi,logpt,c4,c5\n";
    // const auto view = host_tensor.const_view();
    // for (auto ii = 0; ii < view.metadata().size(); ++ii) {
    //   const auto features = view[ii].features();
    //   for (auto row = 0; row < PuppiFeatures::RowsAtCompileTime; ++row) {
    //     out_stream << ii << "," << row;
    //     for (auto col = 0; col < PuppiFeatures::ColsAtCompileTime; ++col)
    //       out_stream << "," << features(row, col);
    //     out_stream << "\n";
    //   }
    // }
    /* END DEBUG */

    return input_tensor;
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::l1sc::kernels
