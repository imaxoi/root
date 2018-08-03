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

#include <vector>
#include <string>
#include <sstream>

namespace ROOT {
namespace Internal {
namespace RDF {
class RDisplayer {
   using Row_t = std::vector<std::string>;

private:
   static constexpr char fgSeparator = ' ';
   std::vector<Row_t> fTable;
   std::vector<unsigned short> fWidths;
   int fNColumns;
   int fCurrentRow = 0;
   int fCurrentColumn = 0;

   void UpdateWidth(const std::string &element)
   {
      auto length = element.length();

      if (fWidths[fCurrentColumn] < length) {
         fWidths[fCurrentColumn] = length;
      }
   }

   void MoveToNextElement()
   {
      ++fCurrentColumn;
      if (fCurrentColumn == fNColumns) {
         ++fCurrentRow;
         fCurrentColumn = 0;
         fTable.push_back(Row_t(fNColumns));
      }
   }

public:
   RDisplayer(const Row_t &columnNames) : fWidths(columnNames.size(), 0), fNColumns(columnNames.size())
   {
      fTable.push_back(Row_t(columnNames.size()));
      for (auto name : columnNames) {
         AddToRow(name);
      }
   }

   void AddToRow(const std::string &stringEle)
   {
      UpdateWidth(stringEle);

      fTable[fCurrentRow][fCurrentColumn] = stringEle;
      MoveToNextElement();
   }

   template <typename T, typename std::enable_if<!IsContainer<T>::value, int>::type = 0>
   void AddToRow(const T &ele)
   {
      auto stringEle = std::to_string(ele);
      UpdateWidth(stringEle);

      fTable[fCurrentRow][fCurrentColumn] = stringEle;
      MoveToNextElement();
   }

   template <typename T, typename std::enable_if<IsContainer<T>::value, int>::type = 0>
   void AddToRow(const T &collection)
   {
      for (auto ele : collection) {
         auto stringEle = std::to_string(ele);
         UpdateWidth(stringEle);
         fTable[fCurrentRow][fCurrentColumn] = stringEle;
         ++fCurrentRow;
         fTable.push_back(Row_t(fNColumns));
      }
      MoveToNextElement();
   }

   void Print() const
   {
      for (auto row : fTable) {
         for (int i = 0; i < row.size(); ++i) {
            std::cout << std::left << std::setw(fWidths[i]) << std::setfill(fgSeparator) << row[i] << " | ";
         }
         std::cout << std::endl;
      }
   }
};

} // namespace RDF
} // namespace Internal
} // namespace ROOT

#endif
