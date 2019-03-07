#include "./message.h"

#include <sstream>
#include <boost/algorithm/string.hpp>

#include "./api.h"

using boost::algorithm::replace_all;

namespace cq::message {
    std::string escape(std::string str, const bool escape_comma) {
        replace_all(str, "&", "&amp;");
        replace_all(str, "[", "&#91;");
        replace_all(str, "]", "&#93;");
        if (escape_comma) replace_all(str, ",", "&#44;");
        return str;
    }

    std::string unescape(std::string str) {
        replace_all(str, "&#44;", ",");
        replace_all(str, "&#91;", "[");
        replace_all(str, "&#93;", "]");
        replace_all(str, "&amp;", "&");
        return str;
    }

    Message::Message(const std::string &msg_str) {
        // implement a DFA manually, because the regex lib of VC++ will throw stack overflow in some cases

        const static auto TEXT = 0;
        const static auto FUNCTION_NAME = 1;
        const static auto PARAMS = 2;
        auto state = TEXT;
        const auto end = msg_str.cend();
        std::stringstream text_s, function_name_s, params_s;
        auto curr_cq_start = end;
        for (auto it = msg_str.cbegin(); it != end; ++it) {
            const auto curr = *it;
            switch (state) {
            case TEXT: {
            text:
                if (curr == '[' && end - 1 - it >= 5 /* [CQ:a] at least 5 chars behind */
                    && *(it + 1) == 'C' && *(it + 2) == 'Q' && *(it + 3) == ':' && *(it + 4) != ']') {
                    state = FUNCTION_NAME;
                    curr_cq_start = it;
                    it += 3;
                } else {
                    text_s << curr;
                }
                break;
            }
            case FUNCTION_NAME: {
                if ((curr >= 'A' && curr <= 'Z') || (curr >= 'a' && curr <= 'z') || (curr >= '0' && curr <= '9')) {
                    function_name_s << curr;
                } else if (curr == ',') {
                    // function name out, params in
                    state = PARAMS;
                } else if (curr == ']') {
                    // CQ code end, with no params
                    goto params;
                } else {
                    // unrecognized character
                    text_s << std::string(curr_cq_start, it); // mark as text
                    curr_cq_start = end;
                    function_name_s = std::stringstream();
                    params_s = std::stringstream();
                    state = TEXT;
                    // because the current char may be '[', we goto text part
                    goto text;
                }
                break;
            }
            case PARAMS: {
            params:
                if (curr == ']') {
                    // CQ code end
                    MessageSegment seg;

                    seg.type = function_name_s.str();
                    while (params_s.rdbuf()->in_avail()) {
                        // split key and value
                        std::string key, value;
                        getline(params_s, key, '=');
                        getline(params_s, value, ',');
                        seg.data[key] = unescape(value);
                    }

                    if (text_s.rdbuf()->in_avail()) {
                        // there is a text segment before this CQ code
                        this->push_back(MessageSegment{"text", {{"text", unescape(text_s.str())}}});
                        text_s = std::stringstream();
                    }

                    this->push_back(seg);
                    curr_cq_start = end;
                    text_s = std::stringstream();
                    function_name_s = std::stringstream();
                    params_s = std::stringstream();
                    state = TEXT;
                } else {
                    params_s << curr;
                }
            }
            default:
                break;
            }
        }

        // iterator end, there may be some rest of message we haven't put into segments
        switch (state) {
        case FUNCTION_NAME:
        case PARAMS:
            // we are in CQ code, but it ended with no ']', so it's a text segment
            text_s << std::string(curr_cq_start, end);
            // should fall through
        case TEXT:
            if (text_s.rdbuf()->in_avail()) {
                this->push_back(MessageSegment{"text", {{"text", unescape(text_s.str())}}});
            }
        default:
            break;
        }
    }

    Message::operator std::string() const {
        std::stringstream ss;
        for (auto seg : *this) {
            if (seg.type.empty()) {
                continue;
            }
            if (seg.type == "text") {
                if (const auto it = seg.data.find("text"); it != seg.data.end()) {
                    ss << escape((*it).second, false);
                }
            } else {
                ss << "[CQ:" << seg.type;
                for (const auto &item : seg.data) {
                    ss << "," << item.first << "=" << escape(item.second, true);
                }
                ss << "]";
            }
        }
        return ss.str();
    }

    int64_t Message::send(const Target &target) const { return api::send_msg(target, *this); }

    std::string Message::extract_plain_text() const {
        std::string result;
        for (const auto &seg : *this) {
            if (seg.type == "text") {
                result += seg.data.at("text") + " ";
            }
        }
        if (!result.empty()) {
            result.erase(result.end() - 1); // remove the trailing space
        }
        return result;
    }

    void Message::reduce() {
        if (this->empty()) {
            return;
        }

        auto last_seg_it = this->begin();
        for (auto it = this->begin(); ++it != this->end();) {
            if (it->type == "text" && last_seg_it->type == "text" && it->data.find("text") != it->data.end()
                && last_seg_it->data.find("text") != last_seg_it->data.end()) {
                // found adjacent "text" segments
                last_seg_it->data["text"] += it->data["text"];
                // remove the current element and continue
                this->erase(it);
                it = last_seg_it;
            } else {
                last_seg_it = it;
            }
        }

        if (this->size() == 1 && this->front().type == "text" && this->extract_plain_text().empty()) {
            this->clear(); // the only item is an empty text segment, we should remove it
        }
    }
} // namespace cq::message
