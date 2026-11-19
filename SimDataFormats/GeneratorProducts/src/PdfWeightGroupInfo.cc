#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"

namespace gen {
  PdfWeightGroupInfo* PdfWeightGroupInfo::clone() const { return new PdfWeightGroupInfo(*this); }
  void PdfWeightGroupInfo::addLhaid(int lhaid) {
    lhaids_.push_back(lhaid);
    if (lhaids_.size() == parentLhapdfSize_)
      setIsWellFormed(true);
    else
      setIsWellFormed(false);
  }

  void PdfWeightGroupInfo::setParentLhapdfInfo(int lhaid) {
    parentLhapdfId_ = lhaid;
    LHAPDF::PDFSet pdfSet(LHAPDF::lookupPDF(lhaid).first);
    parentLhapdfSize_ = pdfSet.size();
    parentLhapdfError_ = pdfSet.errorType();
    // Combined error sets append parameter variations, e.g. symmhessian+as.
    const auto coreErrorType = parentLhapdfError_.substr(0, parentLhapdfError_.find('+'));
    if (coreErrorType == "hessian" || coreErrorType == "symmhessian")
      uncertaintyType_ = kHessianUnc;
    else if (coreErrorType == "replicas")
      uncertaintyType_ = kMonteCarloUnc;
    else
      uncertaintyType_ = kUnknownUnc;
  }

}  // namespace gen
