#include "uuid.hpp"

int main() {
    // ��ʼ�� ID ����������ѡ��
    /*bre::IdGenerator::initialize({ .SeqBitLength = 4, .MaxSeqNumber = 10 });*/

    // ���� ID
    for (int i = 0; i < 10; ++i) {
        int64_t id = bre::IdGenerator::Next();
        std::cout << std::format("Generated ID: {}\n", id);
    }

    return 0;
}