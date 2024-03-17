/* © SPIRV-Interpreter @ https://github.com/mmoult/SPIRV-Interpreter
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
module;
#include <sstream>
#include <stdexcept>
#include <vector>

export module value.pointer;
import type;
import value;
import value.aggregate;

export class Pointer : public Value {
    /// @brief an index in Data which all indices point into
    unsigned head;
    /// @brief A list of indices for recursively extracting from the Value at head
    std::vector<unsigned> indices;

public:
    Pointer(unsigned head, std::vector<unsigned>& indices, Type t): Value(t), head(head), indices(indices) {}

    void copyFrom(const Value& new_val) noexcept(false) override {
        Value::copyFrom(new_val);

        // Do the actual copy now
        const Pointer& other = static_cast<const Pointer&>(new_val);
        throw std::runtime_error("Unimplemented function!");
    }

    unsigned getHead() const {
        return head;
    }

    Value* dereference(Value& start) noexcept(false) {
        Value* res = &start;
        for (unsigned idx : indices) {
            if (DataType dt = res->getType().getBase(); dt != DataType::ARRAY && dt != DataType::STRUCT) {
                std::stringstream error;
                error << "Cannot extract from non-composite type!";
                throw std::runtime_error(error.str());
            }
            Aggregate& agg = *static_cast<Aggregate*>(res);
            if (idx >= agg.getSize()) {
                std::stringstream error;
                error << "Index " << idx << " beyond the bound of composite (" << agg.getSize() << ")!";
                throw std::runtime_error(error.str());
            }
            res = agg[idx];
            // Repeat the process for all indices
        }
        return res;
    }

    virtual void print(std::stringstream& dst, unsigned indents = 0) const override {
        dst << "[ " << head;
        for (unsigned u: indices)
            dst << ", " << u;
        dst << " ]";
    }

    bool isNested() const override {
        return true;
    }

    bool equals(const Value& val) const override {
        if (!Value::equals(val)) // guarantees matching types
            return false;
        const auto& other = static_cast<const Pointer&>(val);
        // I cannot think of why this would be used, but implement it in case...
        if ((head != other.head) || (indices.size() != other.indices.size()))
            return false;
        for (unsigned i = 0; i < indices.size(); ++i) {
            if (indices[i] != other.indices[i])
                return false;
        }
        return true;
    }
};
