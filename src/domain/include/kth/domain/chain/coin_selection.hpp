// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_CHAIN_COIN_SELECTION_HPP
#define KTH_DOMAIN_CHAIN_COIN_SELECTION_HPP

namespace kth::domain::chain {

enum class coin_selection_algorithm {
    smallest_first,     // Prioriza UTXOs más pequeños
    largest_first,      // Prioriza UTXOs más grandes
    // knapsack,          // Algoritmo de la mochila para optimizar
    // fifo,              // Primero en entrar, primero en salir
    // branch_and_bound,  // Branch and Bound para optimización global
    manual,            // Mantiene el orden original de los UTXOs
    // privacy,           // Prioriza privacidad (mismo tamaño de UTXOs)
    send_all           // Usa todos los UTXOs disponibles
};

} // namespace kth::domain::chain

#endif