// Author: Enrico Guiraud, Danilo Piparo, Massimo Tumolo CERN  06/2018

/*************************************************************************
 * Copyright (C) 1995-2016, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_RDFBOOKEDCUSTOMCOLUMNS
#define ROOT_RDFBOOKEDCUSTOMCOLUMNS

#include "ROOT/RDFNodes.hxx"
#include "ROOT/RDFUtils.hxx"
#include <vector>
#include <memory>
#include <map>

namespace ROOT {
namespace Internal {
namespace RDF {

struct RBookedCustomColumns{
   using ColumnNames_t = ROOT::Detail::RDF::ColumnNames_t;
   using ColumnNamesPtr_t = std::shared_ptr<ColumnNames_t>;

   using RCustomColumnBasePtrMap_t = std::map<std::string, std::shared_ptr<RCustomColumnBase>>;
   using RCustomColumnBasePtrMapPtr_t = std::shared_ptr<RCustomColumnBasePtrMap_t>;

   RCustomColumnBasePtrMapPtr_t fCustomColumns;
   ColumnNamesPtr_t fCustomColumnsNames;

   RBookedCustomColumns(){
      fCustomColumnsNames=std::make_shared<ColumnNames_t>();
      fCustomColumns=std::make_shared<RCustomColumnBasePtrMap_t>();
   }

};

} // Namespace RDF
} // Namespace Internal
} // Namespace ROOT

#endif
