//
// Created by fatcat on 3/1/19.
//

#include "info.h"

using namespace std;

void assignSum(union Out *res) {
    int value = 0;
    for (int i = 0; i < sizeof(res->data); ++i) {
        unsigned char asone = static_cast<unsigned char>(res->data[i]);
        value += asone;

    }
    memcpy(res->meta.sum, &value, sumNum);
    //cout<<"value:"<<value<<"res:"<<res->meta.sum<<endl;
}

int Info::push(unsigned char od) {
    this->data.push_back(od);
    //test data head
    if (this->ableLength < sizeof(dataHead)) {
        if (od == dataHead[ableLength])
            ableLength++;
        else
            ableLength = 0;
        return 0;
    } else {
        ableLength++;
    }
    if (ableLength == this->inLength) {
        ableLength = 0;
        int sumTest = getSum();
        this->getData();
        int sum = 0;
        memcpy(&sum, result.meta.sum, sumNum);
        //test
        //cout << this->result.data << endl;
        if (sumTest == sum)
            return 1;
        else
            return -1;
    }

    return 0;
}

void Info::getData() {
    auto end = this->data.end();
    for (int i = 1; i <= inLength; ++i) {
        result.data[inLength - i] = *(end - i);
    }
}

int Info::getSum() {
    int res = 0;
    for (auto end = this->data.rbegin() + sumNum; end < this->data.rbegin() + inLength; ++end) {
        int a = *end;
        res += a;
    }
    return res;
}