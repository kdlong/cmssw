#ifndef GENWEIGHTSCOUNTERS_h
#define GENWEIGHTSCOUNTERS_h
#include <map>
#include <string>
#include <vector>
#include "FWCore/Utilities/interface/Exception.h"
namespace genCounter {

  inline void mergeSumVectors(std::vector<long double>& v1,
                              std::vector<long double> const& v2,
                              const std::string& name = "") {
    if (v2.empty())
      return;
    if (v1.empty() && !v2.empty())
      v1.resize(v2.size(), 0);
    if (v1.size() != v2.size())
      throw cms::Exception("WeightCounterSizeMismatch")
          << "Cannot merge weight counter " << name << ": accumulated size " << v1.size() << ", incoming size "
          << v2.size() << ".";
    for (size_t i = 0; i < v1.size(); ++i)
      v1[i] += v2[i];
  }

  ///  ---- Cache object for running sums of weights ----
  class Counter {
  public:
    void clear() {
      num_ = 0;
      sumw_ = 0;
      sumw2_ = 0;
      weightSumMap_.clear();
    }

    // inc only the gen counters
    void incGenOnly(double w) {
      num_++;
      sumw_ += w;
      sumw2_ += (w * w);
    }

    void incLHE(double w0, const std::vector<double>& weightV, const std::string& wName) {
      auto& sums = weightSumMap_[wName];
      if (!weightV.empty()) {
        if (sums.empty())
          sums.resize(weightV.size(), 0);
        if (sums.size() != weightV.size())
          throw cms::Exception("WeightCounterSizeMismatch")
              << "Weight counter " << wName << " changed size: accumulated size " << sums.size() << ", incoming size "
              << weightV.size() << ".";
        for (unsigned int i = 0, n = weightV.size(); i < n; ++i)
          sums[i] += (w0 * weightV[i]);
      }
      // incGenOnly(w0);
      // incPSOnly(w0, wPS);
    }

    void mergeSumMap(const Counter& other) {
      num_ += other.num_;
      sumw_ += other.sumw_;
      sumw2_ += other.sumw2_;
      for (const auto& wmap : other.weightSumMap_)
        mergeSumVectors(weightSumMap_[wmap.first], wmap.second, wmap.first);
    }

    // private:
    // the counters
    long long num_ = 0;
    long double sumw_ = 0;
    long double sumw2_ = 0;
    std::map<std::string, std::vector<long double>> weightSumMap_;
  };

  struct CounterMap {
    std::map<std::string, Counter> countermap;
    Counter* active_el = nullptr;
    std::string active_label = "";

    void mergeSumMap(const CounterMap& other) {
      for (const auto& y : other.countermap) {
        countermap[y.first].mergeSumMap(y.second);
      }
      active_el = nullptr;
    }

    void clear() {
      for (auto& x : countermap)
        x.second.clear();
    }

    void setLabel(std::string label) {
      active_el = &(countermap[label]);
      active_label = label;
    }
    void checkLabelSet() {
      if (!active_el)
        throw cms::Exception("LogicError", "Called CounterMap::get() before setting the active label\n");
    }
    Counter* get() {
      checkLabelSet();
      return active_el;
    }
    std::string& getLabel() {
      checkLabelSet();
      return active_label;
    }
  };

}  // namespace genCounter
#endif
