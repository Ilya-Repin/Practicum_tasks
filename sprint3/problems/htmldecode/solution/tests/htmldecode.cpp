#include <unordered_map>
#include "htmldecode.h"

std::string HtmlDecode(std::string_view str) {
  std::string result;
  std::unordered_map<std::string_view, char> entities{{"&lt", '<'}, {"&LT", '<'},
                                                      {"&gt", '>'}, {"&GT", '>'},
                                                      {"&amp", '&'}, {"&AMP", '&'},
                                                      {"&apos", '\''}, {"&APOS", '\''},
                                                      {"&quot", '"'}, {"&QUOT", '"'},};
  for (size_t i = 0; i < str.size(); ++i) {
    auto symbol = str[i];

    if (symbol == '&') {
      char decoded = '&';
      bool is_mnemonic = false;

      if (entities.contains(str.substr(i, 3))) {
        decoded = entities[str.substr(i, 3)];
        is_mnemonic = true;
        i += 2;
      }
      else if (entities.contains(str.substr(i, 4))) {
        decoded = entities[str.substr(i, 4)];
        is_mnemonic = true;
        i += 3;
      }
      else if (entities.contains(str.substr(i, 5))) {
        decoded = entities[str.substr(i, 5)];
        is_mnemonic = true;
        i += 4;
      }

      i = (str[i + 1] == ';' && is_mnemonic) ? i + 1 : i;
      result.push_back(decoded);

      continue;
    }

    result.push_back(symbol);
  }

  return result;
}
