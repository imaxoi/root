#include "ROOT/RDFBookedCustomColumns.hxx"

namespace ROOT {
namespace Internal {
namespace RDF {

void RBookedCustomColumns::CheckCustomColumn(std::string_view definedCol, TTree *treePtr,
                                             const ColumnNames_t &dataSourceColumns) const
{
   const std::string definedColStr(definedCol);
   if (treePtr != nullptr) {
      // check if definedCol is already present in TTree
      const auto branch = treePtr->GetBranch(definedColStr.c_str());
      if (branch != nullptr) {
         const auto msg = "branch \"" + definedColStr + "\" already present in TTree";
         throw std::runtime_error(msg);
      }
   }
   auto columnNames = GetNames();
   // check if definedCol has already been `Define`d in the functional graph
   if (std::find(columnNames.begin(), columnNames.end(), definedCol) != columnNames.end()) {
      const auto msg = "Redefinition of column \"" + definedColStr + "\"";
      throw std::runtime_error(msg);
   }
   // check if definedCol is already present in the DataSource (but has not yet been `Define`d)
   if (!dataSourceColumns.empty()) {
      if (std::find(dataSourceColumns.begin(), dataSourceColumns.end(), definedCol) != dataSourceColumns.end()) {
         const auto msg = "Redefinition of column \"" + definedColStr + "\" already present in the data-source";
         throw std::runtime_error(msg);
      }
   }
}

bool RBookedCustomColumns::HasName(std::string name) const
{
   return std::find(fCustomColumnsNames->begin(), fCustomColumnsNames->end(), name) != fCustomColumnsNames->end();
}

void RBookedCustomColumns::AddColumn(const std::shared_ptr<RDFDetail::RCustomColumnBase> &column,
                                     const std::string_view &name)
{
   auto newCols = std::make_shared<RCustomColumnBasePtrMap_t>(GetColumns());
   (*newCols)[std::string(name)] = column;
   fCustomColumns = newCols;
}

void RBookedCustomColumns::AddName(const std::string_view &name)
{
   auto newColsNames = std::make_shared<ColumnNames_t>(GetNames());
   newColsNames->emplace_back(name);
   fCustomColumnsNames = newColsNames;
}

} // namespace RDF
} // namespace Internal
} // namespace ROOT
