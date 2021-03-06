/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/header.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

const std::string chain::header::command = "headers";

header header::factory_from_data(const data_chunk& data,
    bool with_transaction_count)
{
    header instance;
    instance.from_data(data, with_transaction_count);
    return instance;
}

header header::factory_from_data(std::istream& stream,
    bool with_transaction_count)
{
    header instance;
    instance.from_data(stream, with_transaction_count);
    return instance;
}

header header::factory_from_data(reader& source,
    bool with_transaction_count)
{
    header instance;
    instance.from_data(source, with_transaction_count);
    return instance;
}

bool header::is_valid() const
{
    return (version != 0) ||
        (previous_block_hash != null_hash) ||
        (merkle != null_hash) ||
        (timestamp != 0) ||
        (bits != 0) ||
        (nonce != 0);
}

void header::reset()
{
    version = 0;
    previous_block_hash.fill(0);
    merkle.fill(0);
    timestamp = 0;
    bits = 0;
    nonce = 0;
}

bool header::from_data(const data_chunk& data,
    bool with_transaction_count)
{
    data_source istream(data);
    return from_data(istream, with_transaction_count);
}

bool header::from_data(std::istream& stream, bool with_transaction_count)
{
    istream_reader source(stream);
    return from_data(source, with_transaction_count);
}

bool header::from_data(reader& source, bool with_transaction_count)
{
    auto result = true;
    reset();
    version = source.read_4_bytes_little_endian();
    previous_block_hash = source.read_hash();
    merkle = source.read_hash();
    timestamp = source.read_4_bytes_little_endian();
    bits = source.read_4_bytes_little_endian();
    nonce = source.read_4_bytes_little_endian();
    transaction_count = 0;
    if (with_transaction_count)
        transaction_count = source.read_variable_uint_little_endian();

    result = source;
    if (!result)
        reset();

    return result;
}

data_chunk header::to_data(bool with_transaction_count) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream, with_transaction_count);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(with_transaction_count));
    return data;
}

void header::to_data(std::ostream& stream,
    bool with_transaction_count) const
{
    ostream_writer sink(stream);
    to_data(sink, with_transaction_count);
}

void header::to_data(writer& sink, bool with_transaction_count) const
{
    sink.write_4_bytes_little_endian(version);
    sink.write_hash(previous_block_hash);
    sink.write_hash(merkle);
    sink.write_4_bytes_little_endian(timestamp);
    sink.write_4_bytes_little_endian(bits);
    sink.write_4_bytes_little_endian(nonce);

    if (with_transaction_count)
        sink.write_variable_uint_little_endian(transaction_count);
}

uint64_t header::serialized_size(bool with_transaction_count) const
{
    uint64_t size = 80;
    if (with_transaction_count)
        size += variable_uint_size(transaction_count);

    return size;
}

hash_digest header::hash() const
{
    return bitcoin_hash(to_data(false));
}

bool operator==(const header& left, const header& right)
{
    return (left.version == right.version)
        && (left.previous_block_hash == right.previous_block_hash)
        && (left.merkle == right.merkle)
        && (left.timestamp == right.timestamp)
        && (left.bits == right.bits)
        && (left.nonce == right.nonce)
        && (left.transaction_count == right.transaction_count);
}

bool operator!=(const header& left, const header& right)
{
    return !(left == right);
}

} // namspace chain
} // namspace libbitcoin
