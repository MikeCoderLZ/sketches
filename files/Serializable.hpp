#pragma once

#include <memory>

namespace geni::files {

  template< typename SerialTarget >
  class Serializable {
    public:
      using Target = std::unique_ptr<SerialTarget>;

      virtual
      auto save() const -> Target =0;
      virtual
      void load( SerialTarget& saveData ) =0;
  };

}