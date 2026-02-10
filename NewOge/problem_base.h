// problem_base.h
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <ctime>

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
        int seed = 0;
        std::unordered_map<std::string, std::string> custom_params;

        int get_int_param(const std::string& key, int default_value = 0) const;
        std::string get_string_param(const std::string& key,
            const std::string& default_value = "") const;
        bool get_bool_param(const std::string& key, bool default_value = false) const;
    };

    class ProblemBase {
    protected:
        ProblemMeta meta_;
        std::string unique_id_;
        std::string problem_text_;
        std::string correct_answer_;
        std::string solution_;
        std::string hint_;
        std::time_t generated_at_;
        int score_;

    public:
        ProblemBase(const ProblemMeta& meta);
        virtual ~ProblemBase() = default;

        virtual void generate(const GenerationConfig& config) = 0;
        virtual bool check_answer(const std::string& user_answer) const = 0;
        virtual std::string to_html(bool show_solution = false) const = 0;

        virtual std::string to_json() const;

        const std::string& get_unique_id() const { return unique_id_; }
        const std::string& get_problem_text() const { return problem_text_; }
        const std::string& get_correct_answer() const { return correct_answer_; }
        const std::string& get_solution() const { return solution_; }
        const std::string& get_hint() const { return hint_; }
        const ProblemMeta& get_meta() const { return meta_; }
        int get_score() const { return score_; }
        std::time_t get_generation_time() const { return generated_at_; }

        void set_unique_id(const std::string& id) { unique_id_ = id; }
        void set_score(int score) { score_ = score; }

    protected:
        std::string generate_unique_id() const;
        void validate_answer(const std::string& answer) const;
    };

} // namespace OGE