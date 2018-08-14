// Author: Enrico Guiraud, Danilo Piparo CERN, Massimo Tumolo Politecnico di Torino  08/2018

/*************************************************************************
 * Copyright (C) 1995-2016, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_RDFDISPLAYER
#define ROOT_RDFDISPLAYER

#include "ROOT/TypeTraits.hxx"
#include "TInterpreter.h"

#include <vector>
#include <string>
#include <sstream>

namespace ROOT {
namespace Internal {
namespace RDF {
std::string PrettyPrintAddr(const void *const addr);

class RDisplayer {
private:
   using Row_t = std::vector<std::string>;
   using Types_t = std::vector<std::string>;
   static constexpr char fgSeparator = ' ';

   Types_t fTypes;
   std::vector<std::string> fRepresentations;

   std::vector<Row_t> fTable;
   std::vector<unsigned short> fWidths;
   int fNColumns;
   int fCurrentRow = 0;
   int fCurrentColumn = 0;

   template <typename T>
   bool AddInterpreterString(std::stringstream &stream, T &element, const int &index)
   {
      stream << "*((std::string*)" << &(fRepresentations[index]) << ") = cling::printValue((" << fTypes[index] << "*)"
             << PrettyPrintAddr(&element) << ");";
      return true;
   }

   void AddToRow(const std::string &stringEle)
   {
      if (fWidths[fCurrentColumn] < stringEle.length()){
         fWidths[fCurrentColumn]= stringEle.length();
      }

      fTable[fCurrentRow][fCurrentColumn]  = stringEle;
      ++fCurrentColumn;
      if(fCurrentColumn == fNColumns){
         ++fCurrentRow;
         fCurrentColumn=0;
         fTable.push_back(Row_t(fNColumns));
      }
   }


public:
   RDisplayer(const Row_t &columnNames, const Types_t &types)
      : fWidths(columnNames.size(), 0), fNColumns(columnNames.size()), fTypes(types),
        fRepresentations(columnNames.size())
   {
      fTable.push_back(Row_t(columnNames.size()));
      for (auto name : columnNames){
         AddToRow(name);
      }
   }

   template <typename FirstColumn, typename... OtherColumns>
   void AddRow(FirstColumn first, OtherColumns... column)
   {
      std::stringstream calc;
      bool unpack[] = {AddInterpreterString(calc, first, 0), AddInterpreterString(calc, column, 1)...};
      TInterpreter::EErrorCode errorCode;
      gInterpreter->Calc(calc.str().c_str(), &errorCode);
      if (TInterpreter::EErrorCode::kNoError != errorCode) {
         std::string msg = "Cannot jit during Display call. Interpreter error code is " + std::to_string(errorCode) + ".";
         throw std::runtime_error(msg);
      }

      for(auto name : fRepresentations){
         AddToRow(name);
      }


   };

   void Print() const {
      for (auto row : fTable){
         for (int i = 0; i< row.size(); ++i){
            std::cout << std::left << std::setw(fWidths[i]) << std::setfill(fgSeparator) << row[i] <<" | ";
         }
         std::cout <<std::endl;
      }
   }
};

} // namespace RDF
} // namespace Internal
} // namespace ROOT

#endif
