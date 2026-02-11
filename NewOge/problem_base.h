// problem_base.h
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <ctime>
#include"page_setting.h"

namespace OGE {

    enum class Difficulty {
        EASY = 1,
        MEDIUM = 2,
        HARD = 3
    };

    struct ProblemMeta {
        std::string type_id;
        std::string display_name;
        int problem_number;
        int default_score;
        std::vector<std::string> tags;
        bool is_active;
    };

    struct GenerationConfig {
        Difficulty difficulty = Difficulty::MEDIUM;

        std::unordered_map<std::string, std::string> custom_params;

        std::string get_string_param(const std::string& key,
            const std::string& default_value = "") const;

    };

    class ProblemBase {
    protected:
        ProblemMeta meta;
        std::string unique_id;
        std::string problem_text;
        std::string correct_answer;
        std::string solution;
        std::string hint;
        std::time_t generated_at;
        int score;

    public:
        ProblemBase(const ProblemMeta& meta);
        virtual ~ProblemBase() = default;

        virtual void generate(const GenerationConfig& config) = 0;

        virtual std::string generate_html() const = 0;

        const std::string& get_unique_id() const { return unique_id; }
        const std::string& get_problem_text() const { return problem_text; }
        const std::string& get_correct_answer() const { return correct_answer; }
        const std::string& get_solution() const { return solution; }
        const std::string& get_hint() const { return hint; }
        const ProblemMeta& get_meta() const { return meta; }
        int get_score() const { return score; }
        std::time_t get_generation_time() const { return generated_at; }

        void set_unique_id(const std::string& id) { unique_id = id; }
        void set_score(int score) { score = score; }

    protected:
        std::string generate_unique_id() const;
        void validate_answer(const std::string& answer) const;
    };

} // namespace OGE