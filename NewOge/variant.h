// variant.h
#pragma once

#include "problem_base.h"
#include <vector>
#include <memory>

namespace OGE {

    class Variant {
    private:
        std::string variant_id_;
        std::vector<std::unique_ptr<ProblemBase>> problems_;
        std::time_t created_at_;
        int total_score_;
        std::string teacher_id_;

    public:
        Variant(const std::string& teacher_id = "");

        void add_problem(std::unique_ptr<ProblemBase> problem);

        const std::string& get_id() const { return variant_id_; }
        const std::vector<std::unique_ptr<ProblemBase>>& get_problems() const { return problems_; }
        size_t size() const { return problems_.size(); }
        int get_total_score() const { return total_score_; }
        std::time_t get_creation_time() const { return created_at_; }
        const std::string& get_teacher_id() const { return teacher_id_; }

        const ProblemBase* get_problem(size_t index) const;
        ProblemBase* get_problem(size_t index);

        std::string to_html(bool show_solutions = false) const;
        std::string to_json() const;
        bool save_to_file(const std::string& filename) const;
        static std::unique_ptr<Variant> load_from_file(const std::string& filename);

        struct Statistics {
            int total_problems = 0;
            int total_score = 0;
            std::unordered_map<int, int> problems_by_number;
            std::unordered_map<Difficulty, int> problems_by_difficulty;
        };

        Statistics get_statistics() const;

    private:
        void update_total_score();
        std::string generate_variant_id() const;
    };

} // namespace OGE