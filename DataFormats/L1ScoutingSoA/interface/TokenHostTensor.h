#ifndef DataFormats_L1ScoutingSoA_interface_TokenHostTensor_h
#define DataFormats_L1ScoutingSoA_interface_TokenHostTensor_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "DataFormats/L1ScoutingSoA/interface/TokenTensorSoA.h"

namespace l1sc {

  using TokenInputHostTensor = PortableHostCollection<TokenInputTensorSoA>;
  using TokenOutputHostTensor = PortableHostCollection<TokenOutputTensorSoA>;

}  // namespace l1sc

#endif  // DataFormats_L1ScoutingSoA_interface_TokenHostTensor_h