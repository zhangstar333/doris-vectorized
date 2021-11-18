// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include "common/logging.h"

#include "vec/aggregate_functions/aggregate_function.h"
#include "vec/columns/column_vector.h"
#include "vec/columns/column_decimal.h"
#include "vec/common/assert_cast.h"
#include "vec/io/io_helper.h"

namespace doris::vectorized {

/// For numeric values.
template <typename T>
struct SingleValueDataFixed {
private:
    using Self = SingleValueDataFixed;

    bool has_value =
            false; /// We need to remember if at least one value has been passed. This is necessary for AggregateFunctionIf.
    T value;

public:
    bool has() const { return has_value; }

    void insert_result_into(IColumn& to) const {
        if (has())
            assert_cast<ColumnVector<T>&>(to).get_data().push_back(value);
        else
            assert_cast<ColumnVector<T>&>(to).insert_default();
    }

    void reset() {
        if (has()) {
            has_value = false;    
        }
    }

    void write(BufferWritable& buf) const {
        write_binary(has(), buf);
        if (has()) write_binary(value, buf);
    }

    void read(BufferReadable& buf) {
        read_binary(has_value, buf);
        if (has()) read_binary(value, buf);
    }

    void change(const IColumn& column, size_t row_num, Arena*) {
        has_value = true;
        value = assert_cast<const ColumnVector<T>&>(column).get_data()[row_num];
    }

    /// Assuming to.has()
    void change(const Self& to, Arena*) {
        has_value = true;
        value = to.value;
    }

    bool change_first_time(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has()) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_first_time(const Self& to, Arena* arena) {
        if (!has() && to.has()) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_every_time(const IColumn& column, size_t row_num, Arena* arena) {
        change(column, row_num, arena);
        return true;
    }

    bool change_every_time(const Self& to, Arena* arena) {
        if (to.has()) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_if_less(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has() || assert_cast<const ColumnVector<T>&>(column).get_data()[row_num] < value) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_if_less(const Self& to, Arena* arena) {
        if (to.has() && (!has() || to.value < value)) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_if_greater(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has() || assert_cast<const ColumnVector<T>&>(column).get_data()[row_num] > value) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_if_greater(const Self& to, Arena* arena) {
        if (to.has() && (!has() || to.value > value)) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool is_equal_to(const Self& to) const { return has() && to.value == value; }

    bool is_equal_to(const IColumn& column, size_t row_num) const {
        return has() && assert_cast<const ColumnVector<T>&>(column).get_data()[row_num] == value;
    }
};

/// For numeric values.
template <>
struct SingleValueDataFixed<DecimalV2Value> {
private:
    using Self = SingleValueDataFixed;

    bool has_value = false; /// We need to remember if at least one value has been passed. This is necessary for AggregateFunctionIf.
    int128_t value;

public:
    bool has() const { return has_value; }

    void insert_result_into(IColumn& to) const {
        if (has()) {
            DecimalV2Value decimal(value);
            assert_cast<ColumnDecimal<Decimal128> &>(to).insert_data((const char *) &decimal, 0);
        }
        else
            assert_cast<ColumnDecimal<Decimal128>&>(to).insert_default();
    }

    void reset() {
        if (has()) {
            has_value = false;    
        }
    }
    
    void write(BufferWritable& buf) const {
        write_binary(has(), buf);
        if (has()) write_binary(value, buf);
    }

    void read(BufferReadable& buf) {
        read_binary(has_value, buf);
        if (has()) read_binary(value, buf);
    }

    void change(const IColumn& column, size_t row_num, Arena*) {
        has_value = true;
        value = assert_cast<const ColumnDecimal<Decimal128>&>(column).get_data()[row_num];
    }

    /// Assuming to.has()
    void change(const Self& to, Arena*) {
        has_value = true;
        value = to.value;
    }

    bool change_first_time(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has()) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_first_time(const Self& to, Arena* arena) {
        if (!has() && to.has()) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_every_time(const IColumn& column, size_t row_num, Arena* arena) {
        change(column, row_num, arena);
        return true;
    }

    bool change_every_time(const Self& to, Arena* arena) {
        if (to.has()) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_if_less(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has() || assert_cast<const ColumnDecimal<Decimal128>&>(column).get_data()[row_num] < value) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_if_less(const Self& to, Arena* arena) {
        if (to.has() && (!has() || to.value < value)) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_if_greater(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has() || assert_cast<const ColumnDecimal<Decimal128>&>(column).get_data()[row_num] > value) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_if_greater(const Self& to, Arena* arena) {
        if (to.has() && (!has() || to.value > value)) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool is_equal_to(const Self& to) const { return has() && to.value == value; }

    bool is_equal_to(const IColumn& column, size_t row_num) const {
        return has() && assert_cast<const ColumnDecimal<Decimal128>&>(column).get_data()[row_num] == value;
    }
};

/** For strings. Short strings are stored in the object itself, and long strings are allocated separately.
  * NOTE It could also be suitable for arrays of numbers.
  */
struct SingleValueDataString {
private:
    using Self = SingleValueDataString;

    Int32 size = -1;    /// -1 indicates that there is no value.
    Int32 capacity = 0; /// power of two or zero
    char* large_data = nullptr;

public:
    static constexpr Int32 AUTOMATIC_STORAGE_SIZE = 64;
    static constexpr Int32 MAX_SMALL_STRING_SIZE =
            AUTOMATIC_STORAGE_SIZE - sizeof(size) - sizeof(capacity) - sizeof(large_data);

private:
    char small_data[MAX_SMALL_STRING_SIZE]; /// Including the terminating zero.

public:
    ~SingleValueDataString() {
        delete large_data;
    }

    bool has() const { return size >= 0; }

    const char* get_data() const { return size <= MAX_SMALL_STRING_SIZE ? small_data : large_data; }

    void insert_result_into(IColumn& to) const {
        if (has())
            assert_cast<ColumnString &>(to).insert_data(get_data(), size);
        else
            assert_cast<ColumnString &>(to).insert_default();
    }

    void reset() {
        if (size != -1) {
            size = -1;    
            capacity = 0; 
            large_data = nullptr;
        }
    }
    
    void write(BufferWritable& buf) const {
        write_binary(size, buf);
        if (has()) buf.write(get_data(), size);
    }

    void read(BufferReadable& buf) {
        Int32 rhs_size;
        read_binary(rhs_size, buf);

        if (rhs_size >= 0)
        {
            if (rhs_size <= MAX_SMALL_STRING_SIZE)
            {
                /// Don't free large_data here.

                size = rhs_size;

                if (size > 0)
                    buf.read(small_data, size);
            }
            else
            {
                if (capacity < rhs_size)
                {
                    capacity = static_cast<UInt32>(round_up_to_power_of_two_or_zero(rhs_size));
                    delete large_data;
                    large_data = new char[capacity];
                }

                size = rhs_size;
                buf.read(large_data, size);
            }
        }
        else
        {
            /// Don't free large_data here.
            size = rhs_size;
        }
    }

    StringRef get_string_ref() const { return StringRef(get_data(), size); }

    /// Assuming to.has()
    void change_impl(StringRef value, Arena* arena) {
        Int32 value_size = value.size;

        if (value_size <= MAX_SMALL_STRING_SIZE)
        {
            /// Don't free large_data here.
            size = value_size;

            if (size > 0)
                memcpy(small_data, value.data, size);
        }
        else
        {
            if (capacity < value_size)
            {
                /// Don't free large_data here.
                capacity = round_up_to_power_of_two_or_zero(value_size);
                delete large_data;
                large_data = new char[capacity];
            }

            size = value_size;
            memcpy(large_data, value.data, size);
        }
    }

    void change(const IColumn& column, size_t row_num, Arena* arena) {
        change_impl(assert_cast<const ColumnString &>(column).get_data_at(row_num), arena);
    }

    void change(const Self& to, Arena* arena) { change_impl(to.get_string_ref(), arena); }

    bool change_if_less(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has() || assert_cast<const ColumnString &>(column).get_data_at(row_num) < get_string_ref())
        {
            change(column, row_num, arena);
            return true;
        }
        else
            return false;
    }

    bool change_if_greater(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has() || assert_cast<const ColumnString &>(column).get_data_at(row_num) > get_string_ref())
        {
            change(column, row_num, arena);
            return true;
        }
        else
            return false;
    }

    bool change_first_time(const IColumn& column, size_t row_num, Arena* arena) {
        if (!has()) {
            change(column, row_num, arena);
            return true;
        } else
            return false;
    }

    bool change_first_time(const Self& to, Arena* arena) {
        if (!has() && to.has()) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_every_time(const IColumn& column, size_t row_num, Arena* arena) {
        change(column, row_num, arena);
        return true;
    }

    bool change_every_time(const Self& to, Arena* arena) {
        if (to.has()) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_if_less(const Self& to, Arena* arena) {
        if (to.has() && (!has() || to.get_string_ref() < get_string_ref())) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool change_if_greater(const Self& to, Arena* arena) {
        if (to.has() && (!has() || to.get_string_ref() > get_string_ref())) {
            change(to, arena);
            return true;
        } else
            return false;
    }

    bool is_equal_to(const Self& to) const {
        return has() && to.get_string_ref() == get_string_ref();
    }

    bool is_equal_to(const IColumn& column, size_t row_num) const { return false; }
};


template <typename Data>
struct AggregateFunctionMaxData : Data {
    using Self = AggregateFunctionMaxData;

    bool change_if_better(const IColumn& column, size_t row_num, Arena* arena) {
        return this->change_if_greater(column, row_num, arena);
    }
    bool change_if_better(const Self& to, Arena* arena) {
        return this->change_if_greater(to, arena);
    }

    static const char* name() { return "max"; }
};

template <typename Data>
struct AggregateFunctionMinData : Data {
    using Self = AggregateFunctionMinData;

    bool change_if_better(const IColumn& column, size_t row_num, Arena* arena) {
        return this->change_if_less(column, row_num, arena);
    }
    bool change_if_better(const Self& to, Arena* arena) { return this->change_if_less(to, arena); }

    static const char* name() { return "min"; }
};

template <typename Data>
struct AggregateFunctionAnyData : Data {
    using Self = AggregateFunctionAnyData;

    bool change_if_better(const IColumn& column, size_t row_num, Arena* arena) {
        return this->change_first_time(column, row_num, arena);
    }
    bool change_if_better(const Self& to, Arena* arena) { return this->change_first_time(to, arena); }

    static const char* name() { return "first_value"; }
};

template <typename Data>
struct AggregateFunctionAnyLastData : Data {
    using Self = AggregateFunctionAnyLastData;

    bool change_if_better(const IColumn& column, size_t row_num, Arena* arena) {
        return this->change_every_time(column, row_num, arena);
    }
    bool change_if_better(const Self& to, Arena* arena) {
        return this->change_every_time(to, arena);
    }

    static const char* name() { return "last_value"; }
};

template <typename Data, bool AllocatesMemoryInArena>
class AggregateFunctionsSingleValue final
        : public IAggregateFunctionDataHelper<
                  Data, AggregateFunctionsSingleValue<Data, AllocatesMemoryInArena>> {
private:
    DataTypePtr& type;

public:
    AggregateFunctionsSingleValue(const DataTypePtr& type_)
            : IAggregateFunctionDataHelper<
                      Data, AggregateFunctionsSingleValue<Data, AllocatesMemoryInArena>>({type_},
                                                                                         {}),
              type(this->argument_types[0]) {
        if (StringRef(Data::name()) == StringRef("min") ||
            StringRef(Data::name()) == StringRef("max")) {
            if (!type->is_comparable()) {
                LOG(FATAL) << fmt::format(
                        "Illegal type {} of argument of aggregate function {} because the values "
                        "of that data type are not comparable",
                        type->get_name(), get_name());
            }
        }
    }

    String get_name() const override { return Data::name(); }

    DataTypePtr get_return_type() const override { return type; }

    void add(AggregateDataPtr place, const IColumn** columns, size_t row_num,
             Arena* arena) const override {
        this->data(place).change_if_better(*columns[0], row_num, arena);
    }

    void reset(AggregateDataPtr place) const override {
        this->data(place).reset();
    }

    void merge(AggregateDataPtr place, ConstAggregateDataPtr rhs, Arena* arena) const override {
        this->data(place).change_if_better(this->data(rhs), arena);
    }

    void serialize(ConstAggregateDataPtr place, BufferWritable& buf) const override {
        this->data(place).write(buf);
    }

    void deserialize(AggregateDataPtr place, BufferReadable& buf, Arena*) const override {
        this->data(place).read(buf);
    }

    bool allocates_memory_in_arena() const override { return AllocatesMemoryInArena; }

    void insert_result_into(ConstAggregateDataPtr place, IColumn& to) const override {
        this->data(place).insert_result_into(to);
    }

    const char* get_header_file_path() const override { return __FILE__; }
};

} // namespace doris::vectorized