#pragma once

#include <assert.h>
#include <map>
#include <memory>
#include <optional>
#include <re2/re2.h>

namespace emcc {

class Recognizer {
  const int kind_;

public:
  bool operator<(const Recognizer &rhs) const { return kind_ < rhs.kind_; }

  Recognizer(int kind) : kind_(kind) {}

  int kind() const { return kind_; }

  virtual bool Recognize(re2::StringPiece *input, std::string *lexeme) = 0;

  virtual ~Recognizer() {}
};

class RegexRecognizer : public Recognizer {
  re2::RE2 re_;

public:
  RegexRecognizer(int kind, const std::string &re)
      : Recognizer(kind), re_(re) {}

  bool Recognize(re2::StringPiece *input, std::string *lexeme) override {
    return re2::RE2::Consume(input, re_, lexeme);
  }
};

class CStringRecognizer : public Recognizer {
public:
  CStringRecognizer(int kind) : Recognizer(kind) {}
  bool Recognize(re2::StringPiece *input, std::string *lexeme) override;
};

class Lexer {
protected:
  re2::StringPiece input_;
  std::map<int, std::unique_ptr<Recognizer>> recognizers_;
  re2::RE2 whitespaces_;
  int current_kind_;
  std::string current_lexeme_;
  bool auto_skip_whitespaces_;

  void PrepareLex() {
    if (auto_skip_whitespaces_)
      SkipWhitespaces();
    current_lexeme_.clear();
  }

public:
  enum Kind {
    Eof,
    Error,
    CustomKindStart,
  };

  Lexer() : whitespaces_("\\s+"), auto_skip_whitespaces_(false) {}

  int current_kind() const { return current_kind_; }

  void SetAutoSkipWhitespaces(bool value) { auto_skip_whitespaces_ = value; }

  const std::string &current_lexeme() { return current_lexeme_; }

  void SetInput(re2::StringPiece input) { input_ = input; }

  re2::StringPiece input() const { return input_; }

  bool Ends() const { return input_.empty(); }

  void AddPattern(int kind, const std::string &re) {
    recognizers_.emplace(std::make_pair(
        kind, std::make_unique<RegexRecognizer>(kind, "(" + re + ")")));
  }

  void AddRecognizer(Recognizer *recognizer) {
    recognizers_.emplace(std::make_pair(
        recognizer->kind(), std::unique_ptr<Recognizer>(recognizer)));
    ;
  }

  bool SkipWhitespaces() { return re2::RE2::Consume(&input_, whitespaces_); }

  bool LexIfPresent(int kind) {
    PrepareLex();
    if (Ends()) {
      current_kind_ = Kind::Eof;
      return false;
    }
    auto rec = recognizers_.find(kind);
    if (rec == recognizers_.end()) {
      current_kind_ = Kind::Error;
      return false;
    }
    re2::StringPiece temp = input();
    if (rec->second->Recognize(&temp, &current_lexeme_)) {
      input_ = temp;
      current_kind_ = rec->second->kind();
      assert(current_kind_ == kind);
      return true;
    }
    current_kind_ = Kind::Error;
    return false;
  }

  int Lex() {
    PrepareLex();
    if (Ends())
      return current_kind_ = Kind::Eof;
    for (auto &rec : recognizers_) {
      re2::StringPiece temp = input();
      if (rec.second->Recognize(&temp, &current_lexeme_)) {
        input_ = temp;
        return current_kind_ = rec.second->kind();
      }
    }
    return current_kind_ = Kind::Error;
  }
};

} // namespace emcc
