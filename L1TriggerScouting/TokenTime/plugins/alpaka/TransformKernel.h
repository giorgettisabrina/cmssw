#ifndef L1TriggerScouting_TokenTime_plugins_alpaka_TransformKernel_h
#define L1TriggerScouting_TokenTime_plugins_alpaka_TransformKernel_h

// Eigen must be included before any SoA header, to enable the SOA_EIGEN_COLUMN support
#include <Eigen/Core>
#include <Eigen/Dense>

#include <alpaka/alpaka.hpp>

#include "DataFormats/L1ScoutingSoA/interface/alpaka/BxLookupDevice.h"
#include "DataFormats/L1ScoutingSoA/interface/alpaka/PuppiDeviceCollection.h"
#include "DataFormats/L1ScoutingSoA/interface/alpaka/TokenDeviceTensor.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::l1sc::kernels {

  using namespace ::l1sc;

  // Build one [128, N] input matrix per bx from the PUPPI candidates.
  // bx_lookup.offset()[bx] .. bx_lookup.offset()[bx + 1] is the candidate range of each bx.
  TokenInputDeviceTensor transform(Queue& queue, const PuppiDeviceCollection& puppi, const BxLookupDevice& bx_lookup);

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::l1sc::kernels

#endif  // L1TriggerScouting_TokenTime_plugins_alpaka_TransformKernel_h
