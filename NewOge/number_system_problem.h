// number_system_problem.h (ПРИМЕР конкретной задачи)
#pragma once

#include "problem_base.h"
#include <random>

namespace OGE {

    class ConvertToDecimalProblem : public ProblemBase {
    private:
        std::mt19937 rng_;
        int from_base_;
        std::string number_;

    public:
        ConvertToDecimalProblem();

        void generate(const GenerationConfig& config) override;
        bool check_answer(const std::string& user_answer) const override;
        std::string to_html(bool show_solution = false) const override;

    private:
        int random_base(const GenerationConfig& config);
        std::string random_number(int base);
        std::string convert_to_decimal(const std::string& number, int base);
        std::string generate_solution() const;
    };

    // Макрос для упрощения регистрации
#define REGISTER_PROBLEM(ClassName, TypeId, DisplayName, ProblemNum, Score, ...) \
    namespace { \
        struct ClassName##_Registrar { \
            ClassName##_Registrar() { \
                ProblemMeta meta = { \
                    .type_id = TypeId, \
                    .display_name = DisplayName, \
                    .problem_number = ProblemNum, \
                    .default_score = Score, \
                    .tags = {__VA_ARGS__}, \
                    .is_active = true \
                }; \
                ProblemRegistry::register_type(meta, []() { \
                    return std::make_unique<ClassName>(); \
                }); \
            } \
        }; \
        static ClassName##_Registrar ClassName##_registrar; \
    }

} // namespace OGE