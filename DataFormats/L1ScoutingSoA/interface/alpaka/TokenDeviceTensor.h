#ifndef DataFormats_L1ScoutingSoA_interface_alpaka_TokenDeviceTensor_h
#define DataFormats_L1ScoutingSoA_interface_alpaka_TokenDeviceTensor_h

#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "DataFormats/L1ScoutingSoA/interface/TokenHostTensor.h"
#include "DataFormats/L1ScoutingSoA/interface/TokenTensorSoA.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  namespace l1sc {

    // make the names from the top-level portabletest namespace visible for unqualified lookup
    // inside the ALPAKA_ACCELERATOR_NAMESPACE::portabletest namespace
    using namespace ::l1sc;

    using TokenInputDeviceTensor = PortableCollection<TokenInputTensorSoA>;
    using TokenOutputDeviceTensor = PortableCollection<TokenOutputTensorSoA>;

  }  // namespace l1sc

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// heterogeneous ml data checks
ASSERT_DEVICE_MATCHES_HOST_COLLECTION(l1sc::TokenInputDeviceTensor,
                                      l1sc::TokenInputHostTensor);
ASSERT_DEVICE_MATCHES_HOST_COLLECTION(l1sc::TokenOutputDeviceTensor,
                                      l1sc::TokenOutputHostTensor);

#endif  // DataFormats_L1ScoutingSoA_interface_alpaka_TokenDeviceTensor_h