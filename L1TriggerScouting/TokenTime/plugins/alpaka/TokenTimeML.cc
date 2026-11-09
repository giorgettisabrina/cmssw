// Eigen must be included before any SoA header, to enable the SOA_EIGEN_COLUMN support
#include <Eigen/Core>
#include <Eigen/Dense>

#include <cassert>

#include "DataFormats/L1ScoutingSoA/interface/alpaka/BxLookupDevice.h"
#include "DataFormats/L1ScoutingSoA/interface/alpaka/PuppiDeviceCollection.h"
#include "DataFormats/L1ScoutingSoA/interface/alpaka/TokenDeviceTensor.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDGetToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDPutToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/Event.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EventSetup.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/FixedQueueEDProducer.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "L1TriggerScouting/TokenTime/plugins/alpaka/TransformKernel.h"
#include "PhysicsTools/PyTorchAlpaka/interface/BatchedTensorCollection.h"
#include "PhysicsTools/PyTorchAlpaka/interface/TensorCollection.h"
#include "PhysicsTools/PyTorchAlpaka/interface/alpaka/AlpakaModel.h"

#include "PerfTools/Perfetto/interface/CMSSWPerfettoTrace.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::l1sc {

  class TokenTimeML : public stream::FixedQueueEDProducer<> {
  public:
    TokenTimeML(const edm::ParameterSet &params)
        : FixedQueueEDProducer<>(params),
          puppi_candidates_token_(consumes(params.getParameter<edm::InputTag>("srcCandidates"))),
          bx_lookup_token_(consumes(params.getParameter<edm::InputTag>("srcBxLookup"))),
          token_input_token_(produces("tokenInputs")),
          token_output_token_(produces("tokenOutputs")),
          model_(params.getParameter<edm::FileInPath>("model").fullPath()),
          batch_size_(params.getParameter<uint32_t>("batchSize")) {}

    static void fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("srcCandidates");  // PuppiDeviceCollection ("candidates" of the PUPPI unpacker)
      desc.add<edm::InputTag>("srcBxLookup");    // BxLookupDevice ("bxLookup" of the PUPPI unpacker)
      desc.add<edm::FileInPath>("model");
      desc.add<uint32_t>("batchSize", 32u);
      descriptions.addWithDefaultLabel(desc);
    }

    void beginStream(edm::StreamID, Queue queue) {
      // Warming up the model with dummy data. This allows for fast inference since the first iteration
      // and should ensure a more homogeneous memory balance
      TokenInputDeviceTensor input_tensor(queue, batch_size_);    // [B, 128, N] puppi features
      TokenOutputDeviceTensor output_tensor(queue, batch_size_);  // [B, 128] tokens
      input_tensor.zeroInitialise(queue);
      auto input_records = input_tensor.view().records();
      auto output_records = output_tensor.view().records();

      for (auto ii = 0; ii < warmupIterations_; ++ii) {
        cms::torch::alpakatools::TensorCollection<Queue> inputs_d, outputs_d;
        inputs_d.add<TokenInputTensorSoA>("features", input_records.features());
        outputs_d.add<TokenOutputTensorSoA>("embedded", output_records.embedded());
        //::torch::kHalf ---> from fp32 to fp16
        model_.forward(queue, inputs_d, outputs_d, ::torch::kHalf);
      }
    }

    void produce(device::Event &event, const device::EventSetup &event_setup) override {
      CMS_PERFETTO_SCOPE("TokenTimeML produce()");
      const auto &puppi = event.get(puppi_candidates_token_);  // puppi collection
      const auto &bx_lookup = event.get(bx_lookup_token_);     // bx -> candidates offsets

      // fill input tensor: one matrix per bx
      TokenInputDeviceTensor input_tensor = kernels::transform(event.queue(), puppi, bx_lookup);

      // initialize output tensor: one row per bx
      const auto job_size = input_tensor.const_view().metadata().size();
      auto output_tensor = TokenOutputDeviceTensor(event.queue(), job_size);
      output_tensor.zeroInitialise(event.queue());

      if (job_size > 0) {
        CMS_PERFETTO_SCOPE("Run Batched Token Inference");
        assert(batch_size_ > 0 &&
               "batch_size_ is expected to be greater than zero, as unbatched inference will probably make the device "
               "go out of memory");

        // records
        auto input_records = input_tensor.view().records();  // pay A LOT OF attention here to constness
        auto output_records = output_tensor.view().records();

        // batched tensors
        cms::torch::alpakatools::BatchedTensorCollection<Queue> inputs, outputs;
        inputs.addBatched<TokenInputTensorSoA>("features", batch_size_, input_records.features());
        outputs.addBatched<TokenOutputTensorSoA>("embedded", batch_size_, output_records.embedded());

        // do inference
        //::torch::kHalf ---> from fp32 to fp16
        model_.forward(event.queue(), inputs, outputs, ::torch::kHalf);
      }

      event.emplace(token_output_token_, std::move(output_tensor));
      event.emplace(token_input_token_, std::move(input_tensor));
    }

  private:
    // input puppi candidates
    const device::EDGetToken<PuppiDeviceCollection> puppi_candidates_token_;
    // input bx -> candidates offsets
    const device::EDGetToken<BxLookupDevice> bx_lookup_token_;
    // put ml input into event
    const device::EDPutToken<TokenInputDeviceTensor> token_input_token_;
    // put ml output into event
    const device::EDPutToken<TokenOutputDeviceTensor> token_output_token_;
    // model
    torch::AlpakaModel model_;
    // inference batch size (in bx)
    const uint32_t batch_size_;
    // warmup iterations
    const int warmupIterations_ = 10;
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::l1sc

DEFINE_FWK_ALPAKA_MODULE(l1sc::TokenTimeML);
