// problem_base.cpp
#include "problem_base.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace OGE {

    ProblemBase::ProblemBase(const ProblemMeta& meta) : meta(meta) {
        score = meta.default_score;
        generated_at = std::time(nullptr);
    }

    std::string ProblemBase::generate_unique_id() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10000, 99999);

        std::time_t now = std::time(nullptr);
        std::tm tm_now;
        localtime_s(&tm_now, &now);

        std::stringstream ss;
        ss << meta.type_id
            << "_" << meta.problem_number
            << "_" << (tm_now.tm_year + 1900)
            << std::setw(2) << std::setfill('0') << (tm_now.tm_mon + 1)
            << std::setw(2) << std::setfill('0') << tm_now.tm_mday
            << "_" << dis(gen);

        return ss.str();
    }

    void ProblemBase::validate_answer(const std::string& answer) const {
        if (answer.empty()) {
            throw std::runtime_error("Ответ не может быть пустым");
        }
    }

    std::string GenerationConfig::get_string_param(const std::string& key,
        const std::string& default_value) const {
        auto it = custom_params.find(key);
        if (it != custom_params.end()) {
            return it->second;
        }
        return default_value;
    }

}