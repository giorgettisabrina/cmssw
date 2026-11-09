#ifndef DataFormats_L1ScoutingSoA_interface_TokenTensorSoA_h
#define DataFormats_L1ScoutingSoA_interface_TokenTensorSoA_h

#include <Eigen/Core>
#include <Eigen/Dense>

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"

namespace l1sc {

  using PuppiFeatures = Eigen::Matrix<float, 128, 6>;
  //using PaddingMask = Eigen::Vector<bool, 16>;
  GENERATE_SOA_LAYOUT(TokenInputTensorLayout,
                      SOA_EIGEN_COLUMN(PuppiFeatures, features)
                      //, SOA_EIGEN_COLUMN(PaddingMask, pad_mask)
                      )
  using TokenInputTensorSoA = TokenInputTensorLayout<>;

  // one token per candidate row: [0, 4095] (12-bit codebook index), -1 at padding
  using PuppiEmbedding = Eigen::Vector<int32_t, 128>;
  GENERATE_SOA_LAYOUT(TokenOutputTensorLayout,
                      SOA_EIGEN_COLUMN(PuppiEmbedding, embedded))
  using TokenOutputTensorSoA = TokenOutputTensorLayout<>;

}  // namespace l1sc

#endif  // DataFormats_L1ScoutingSoA_interface_TokenTensorSoA_h


// comment: Define input-output tensorSoA