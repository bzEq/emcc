#include "support/lexer.h"

namespace emcc {

bool CStringRecognizer::Recognize(re2::StringPiece *input,
                                  std::string *lexeme) {
  re2::StringPiece &piece = *input;
  if (piece.empty())
    return false;
  const size_t end = piece.size();
  size_t consumed = 0;
  if (piece[consumed++] != '"')
    return false;
  while (consumed != end) {
    char c = piece[consumed];
    if (c == '"')
      break;
    if (c != '\\') {
      lexeme->push_back(c);
      ++consumed;
      continue;
    }
    if (++consumed == end)
      return false;
    c = piece[consumed++];
    switch (c) {
    case '\\':
    case '\'':
    case '"':
      lexeme->push_back(c);
      break;
    case 't':
      lexeme->push_back('\t');
      break;
    case 'n':
      lexeme->push_back('\n');
      break;
    case '\n':
    case '\r':
      break;
    default:
      return false;
    }
  }
  if (consumed == end)
    return false;
  ++consumed;
  piece.remove_prefix(consumed);
  return true;
}

} // namespace emcc
