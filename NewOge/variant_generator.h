// variant_generator.h
#pragma once

#include "variant.h"
#include "problem_registry.h"
#include <random>

namespace OGE {

    class VariantGenerator {
    public:
        struct VariantConfig {
            std::vector<std::string> specific_types;
            std::vector<int> problem_numbers;
            std::vector<std::string> tags;

            int problems_per_type = 1;
            Difficulty difficulty = Difficulty::MEDIUM;

            bool shuffle_problems = true;
            int random_seed = 0;

            int max_total_score = 0;
            int max_total_problems = 0;

            bool validate() const;
        };

        VariantGenerator(const std::string& teacher_id);

        std::unique_ptr<Variant> generate_variant(const VariantConfig& config);
        std::vector<std::unique_ptr<Variant>> generate_variants(
            const VariantConfig& config,
            int count);

        std::unique_ptr<Variant> generate_balanced_variant(
            const std::vector<int>& problem_numbers,
            int total_score_target);

        static std::vector<std::string> get_available_types(
            const VariantConfig& config);

    private:
        std::string teacher_id_;
        std::mt19937 rng_;

        std::vector<std::string> select_problem_types(const VariantConfig& config);
        GenerationConfig create_generation_config(const VariantConfig& variant_config,
            int problem_index);
        void shuffle_problems(std::vector<std::unique_ptr<ProblemBase>>& problems);
        bool check_variant_constraints(const Variant& variant,
            const VariantConfig& config) const;
    };

} // namespace OGE