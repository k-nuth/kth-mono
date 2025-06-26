// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_BLOCKCHAIN_MINING_NODE_V2_HPP_
#define KTH_BLOCKCHAIN_MINING_NODE_V2_HPP_

#include <kth/domain.hpp>

#include <kth/mining/common.hpp>
#include <kth/mining/transaction_element.hpp>

namespace kth {
namespace mining {

class node {
public:

    node(transaction_element const& te)
        : te_(te)
        , children_fees_(te_.fee())
        , children_size_(te_.size())
        , children_sigops_(te_.sigops()) {}

    node(transaction_element&& te)
        : te_(std::move(te))
        , children_fees_(te_.fee())
        , children_size_(te_.size())
        , children_sigops_(te_.sigops()) {}

    transaction_element&& element() {
        return std::move(te_);
    }

    hash_digest const& txid() const {
        return te_.txid();
    }

    uint64_t fee() const {
        return te_.fee();
    }

    size_t sigops() const {
        return te_.sigops();
    }

    size_t size() const {
        return te_.size();
    }

    uint32_t output_count() const {
        return te_.output_count();
    }

    uint64_t children_fees() const {
        return children_fees_;
    }

    size_t children_sigops() const {
        return children_sigops_;
    }

    size_t children_size() const {
        return children_size_;
    }

    std::vector<index_t> const& parents() const {
        return parents_;
    }

    std::vector<index_t>& parents() {
        return parents_;
    }

    std::vector<index_t> const& children() const {
        return children_;
    }

    std::vector<index_t>& children() {
        return children_;
    }

    void add_child(index_t index) {
        children_.push_back(index);
    }

    void remove_child(index_t index) {
        children_.erase(
            std::remove(children_.begin(), children_.end(), index),
            children_.end()
        );
    }

    void add_parent(index_t index) {
        parents_.push_back(index);
    }

    template <typename I>
    void add_parents(I f, I l) {
        parents_.insert(parents_.end(), f, l);
    }

    void remove_parent(index_t index) {
        parents_.erase(
            std::remove(parents_.begin(), parents_.end(), index),
            parents_.end()
        );
    }

    void increment_values(uint64_t fee, size_t size, size_t sigops) {
        children_fees_ += fee;
        children_size_ += size;
        children_sigops_ += sigops;
    }

    void decrement_values(uint64_t fee, size_t size, size_t sigops) {
        children_fees_ -= fee;
        children_size_ -= size;
        children_sigops_ -= sigops;
    }

    void reset_children_values() {
        children_fees_ = fee();
        children_size_ = size();
        children_sigops_ = sigops();
    }

private:
    transaction_element te_;
    std::vector<index_t> parents_;
    std::vector<index_t> children_;

    uint64_t children_fees_;
    size_t children_size_;
    size_t children_sigops_;
};

}  // namespace mining
}  // namespace kth

#endif  //KTH_BLOCKCHAIN_MINING_NODE_V2_HPP_
