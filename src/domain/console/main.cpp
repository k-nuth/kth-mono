#include <iostream>
#include <kth/domain/wallet/payment_address.hpp>

int main() {
    using kth::domain::wallet::payment_address;

    std::cout << "start" << std::endl;
    payment_address const address("bitcoincash:pvstqkm54dtvnpyqxt5m5n7sjsn4enrlxc526xyxlnjkaycdzfeu69reyzmqx");
    std::cout << "address created" << std::endl;
    std::cout << "address is valid: " << bool(address) << std::endl;
    std::cout << "address encoded cashaddr:      " << address.encoded_cashaddr(false) << std::endl;
    std::cout << "address encoded token address: " << address.encoded_cashaddr(true) << std::endl;
    std::cout << "address encoded legacy:        " << address.encoded_legacy() << std::endl;

    return 0;
}
