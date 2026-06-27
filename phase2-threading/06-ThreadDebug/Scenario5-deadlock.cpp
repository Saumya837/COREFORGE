#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

struct Account {
    std::mutex mtx;
    int balance;
};

void transfer(Account& from, Account& to, int amount) {
    std::scoped_lock lock(from.mtx, to.mtx);
    from.balance -= amount;
    to.balance   += amount;
}

int main() {
    Account a{ {}, 1000 };
    Account b{ {}, 1000 };

    // Opposite locking order between the two threads — the deadlock.
    std::thread t1(transfer, std::ref(a), std::ref(b), 100);
    std::thread t2(transfer, std::ref(b), std::ref(a), 50);

    t1.join();
    t2.join();   // never returns until you fix the deadlock

    std::cout << "a=" << a.balance << "  b=" << b.balance << '\n';
    return 0;
}