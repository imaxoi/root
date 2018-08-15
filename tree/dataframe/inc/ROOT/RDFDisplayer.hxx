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
//- TODO!!! THIS SHOULD NOT BE IN ROOT INTERNAL!

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
   std::vector<std::vector<std::string>> fCollectionsRepresentations;
   std::vector<bool> fIsCollection;

   std::vector<Row_t> fTable;
   std::vector<unsigned short> fWidths;
   int fNColumns;
   int fCurrentRow = 0;
   int fNextRow = 0;
   int fCurrentColumn = 0;

   int fEntries;

   template <typename T, typename std::enable_if<!IsContainer<T>::value, int>::type = 0>
   bool AddToInterpreter(std::stringstream &stream, T &element, const int &index)
   {
      stream << "*((std::string*)" << &(fRepresentations[index]) << ") = cling::printValue((" << fTypes[index] << "*)"
             << PrettyPrintAddr(&element) << ");";
      return false;
   }

   template <typename T, typename std::enable_if<IsContainer<T>::value, int>::type = 0>
   bool AddToInterpreter(std::stringstream &stream, T &collection, const int &index)
   {
      int collectionSize = std::distance(std::begin(collection), std::end(collection));
      fCollectionsRepresentations[index] = std::vector<std::string>(collectionSize);

      for(int i=0; i<collectionSize; ++i){
         std::cout << PrettyPrintAddr(&(collection[i]))<<std::endl;
         stream << "*((std::string*)" << &(fCollectionsRepresentations[index][i]) << ") = cling::printValue((" << fTypes[index] << "*)"
                << PrettyPrintAddr(&(collection[i])) << ");";
      }
      return true;
   }


   std::vector<std::string> Split(const std::string &s, const std::string &delimiter)
   {
      std::vector<std::string> tokens;
      auto start = 0U;
      auto end = s.find(delimiter);
      while (end != std::string::npos) {
         tokens.push_back(s.substr(start, end - start));
         start = end + delimiter.length();
         end = s.find(delimiter, start);
      }

      tokens.push_back(s.substr(start, end));
      return tokens;
   }

   void AddToRow(const std::string &stringEle)
   {
      if (fWidths[fCurrentColumn] < stringEle.length()) {
         fWidths[fCurrentColumn] = stringEle.length();
      }

      fTable[fCurrentRow][fCurrentColumn] = stringEle;
      ++fCurrentColumn;
      CheckAndSwitchRow();
   }

   void AddCollectionToRow(const std::vector<std::string> &collection)
   {
      auto row = fCurrentRow;
      for (auto it = collection.begin(); it != collection.end(); ++it) {
         auto stringEle = *it;
         if (fWidths[fCurrentColumn] < stringEle.length()) {
            fWidths[fCurrentColumn] = stringEle.length();
         }
         fTable[row][fCurrentColumn] = stringEle;
         ++row;
         if (std::next(it) != collection.end()) {
            fTable.push_back(Row_t(fNColumns));
         }
      }
      ++fCurrentColumn;
      fNextRow = row;
      CheckAndSwitchRow();
   }

   void CheckAndSwitchRow()
   {
      if (fCurrentColumn == fNColumns) {
         fCurrentRow = fNextRow;
         fCurrentColumn = 0;
         fNextRow = fCurrentRow + 1;
         fTable.push_back(Row_t(fNColumns));
      }
   }

public:
   RDisplayer(const Row_t &columnNames, const Types_t &types, const int &entries)
      : fWidths(columnNames.size(), 0), fNColumns(columnNames.size()), fTypes(types),
        fRepresentations(columnNames.size()), fEntries(entries), fCollectionsRepresentations(columnNames.size())
   {
      fTable.push_back(Row_t(columnNames.size()));
      for (auto name : columnNames) {
         AddToRow(name);
      }
   }

   template <typename FirstColumn, typename... OtherColumns>
   void AddRow(FirstColumn first, OtherColumns... column)
   {
      std::stringstream calc;
      fIsCollection = {AddToInterpreter(calc, first, 0), AddToInterpreter(calc, column, 1)...};

      TInterpreter::EErrorCode errorCode;
      gInterpreter->Calc(calc.str().c_str(), &errorCode);
      if (TInterpreter::EErrorCode::kNoError != errorCode) {
         std::string msg =
            "Cannot jit during Display call. Interpreter error code is " + std::to_string(errorCode) + ".";
         throw std::runtime_error(msg);
      }

      for (int i = 0; i < fNColumns; ++i) {
         if (fIsCollection[i]) {
            AddCollectionToRow(fCollectionsRepresentations[i]);
         } else {
            AddToRow(fRepresentations[i]);
         }
      }

      fEntries--;
   };

   bool HasNext(){
      return fEntries > 0;
   }

   void Print() const
   {
      for (auto row : fTable) {
         std::stringstream stringRow;
         for (int i = 0; i < row.size(); ++i) {
            stringRow << std::left << std::setw(fWidths[i]) << std::setfill(fgSeparator) << row[i] << " | ";
         }
         std::cout << stringRow.str() << std::endl;
      }
   }

   std::string AsString() const
   {
      std::stringstream stringRepresentation;
      for (auto row : fTable) {
         for (int i = 0; i < row.size(); ++i) {
            stringRepresentation << std::left << std::setw(fWidths[i]) << std::setfill(fgSeparator) << row[i] << " | ";
         }
         stringRepresentation << "\n";
      }
      return stringRepresentation.str();
   }
};

} // namespace RDF
} // namespace Internal
} // namespace ROOT

#endif
