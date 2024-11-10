#include <iostream>
#include <cstdint>
#include <ctime>

#if 0
class SnowFlake {
public:
    SnowFlake() {}
private:
    SnowflakeImplement *mSnowflakeImp;
};
#endif

class SnowflakeImplement {
public:
    SnowflakeImplement(uint16_t datacenterId, uint16_t workerId):datacenterId_(datacenterId), workerId_(workerId), sequence_(0), lastTimestamp_(0) {
        if (datacenterId_ > maxDatacenterId_ ||  workerId_ > maxWorkerId_) {
            throw std::invalid_argument("Invaild datacenter or worker ID");
        }
    }

    uint64_t generateUniqueId() {
        uint64_t timestamp = currentTimestamp();
        if (timestamp < lastTimestamp_) {
            throw std::runtime_error("Clock moved backwards");
        } else if (timestamp == lastTimestamp_) {
            sequence_ = (sequence_ + 1) & sequenceMask_;
            if (sequence_ == 0) {
                timestamp = waitNextMillis(lastTimestamp_);
            }
        } else {
            sequence_ = 0;
        }

        lastTimestamp_ = timestamp;
        return ((timestamp - twepoch_) << timestampLeftShift_) |        \
            ((datacenterId_ & maxDatacenterId_) << datacenterIdShift_) | \
            ((workerId_ & maxWorkerId_) << workerIdShift_) |            \
            (sequence_ & sequenceMask_);
    }
    
private:
    uint64_t currentTimestamp() const {
        return std::time(nullptr) * 1000;
    }

    uint64_t waitNextMillis(uint64_t lastTimestamp) {
        uint64_t timestamp = currentTimestamp();
        while (timestamp <= lastTimestamp) {
            timestamp = currentTimestamp();
        }
        return timestamp;
    }
    const uint64_t twepoch_ = 1609459200000ULL;
    const uint64_t unixEpoch = 0ULL;  // start time
    const uint64_t workerIdBits_ = 5;  //  work Id bits
    const uint64_t datacenterIdBits_ = 5;  // data center Id bits
    const uint64_t maxWorkerId_ = (1ULL << workerIdBits_) - 1;
    const uint64_t maxDatacenterId_ = (1ULL << datacenterIdBits_) - 1;
    const uint64_t sequenceBits_ = 12;  // 12 bits sequence Bits
    const uint64_t workerIdShift_ = sequenceBits_;
    const uint64_t datacenterIdShift_ = sequenceBits_ + workerIdBits_;
    const uint64_t timestampLeftShift_ = sequenceBits_ + workerIdBits_ + datacenterIdBits_;
    const uint64_t sequenceMask_ = (1ULL << sequenceBits_) - 1;

    uint16_t datacenterId_;
    uint16_t workerId_;
    uint16_t sequence_;
    uint16_t lastTimestamp_;
};

int main(int argc, char** argv)
{
    uint16_t datacenterId = 1;
    uint16_t workerId = 1;

    SnowflakeImplement snowflakeImplement(datacenterId, workerId);

    for(int i = 0; i < 10; i++) {
        uint64_t uniqueId = snowflakeImplement.generateUniqueId();
        std::cout << "Generated unique ID: " << uniqueId << std::endl;
    }
        
    return 0;
}
