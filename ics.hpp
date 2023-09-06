#pragma once

#include <string>
#include <vector>

template <typename T> struct ics {
public:
    using value_type = T;

public:
    std::vector<T> elts;

public:
    auto contains(T const& e) const -> bool;
    void insert(T const& l, T const& h);
    auto clear();
    template <typename Func> void enumerate(Func&& f) const;

protected:
    struct search_result {
        std::size_t index;
        bool matched;
    };

    auto linear_search(T const& e, std::size_t start_index = 0) const
        -> search_result;
    auto binary_search(T const& e, std::size_t start_index = 0) const
        -> search_result;
};

template <typename T>
auto ics<T>::linear_search(T const& e, std::size_t start_index) const
    -> search_result
{
    auto i = start_index;
    auto const n = elts.size();
    while (i < n && elts[i] < e)
        ++i;
    return {i, (i < n) && (elts[i] == e)};
}

template <typename T>
auto ics<T>::binary_search(T const& e, std::size_t start_index) const
    -> search_result
{
    auto i = start_index;
    auto const n = elts.size();
    auto j = n;
    while (i < j) {
        auto h = int(unsigned(i + j) >> 1); // avoid overflow when computing h
        if (elts[h] < e)
            i = h + 1; // preserves f(i-1) == false
        else
            j = h;
    }
    return {i, (i < n) && (elts[i] == e)};
}

template <typename T> auto ics<T>::contains(T const& e) const -> bool
{
    constexpr auto linear_search_threshold = std::size_t{64};

    auto const sr = elts.size() < linear_search_threshold ? linear_search(e)
                                                          : binary_search(e);

    return ((sr.index & 1) == 0) == sr.matched;
}

template <typename T> void ics<T>::insert(T const& l, T const& h)
{
    auto const n = elts.size();

    if (h <= l) {
        // inserting open-ended interval [l...
        if (n == 0) {
            elts.push_back(l);
            return;
        }
        else if (elts[n - 1] < l) {
            if ((n & 1) == 0) {
                elts.push_back(l);
            }
            return;
        }

        auto const [i, matched] = linear_search(l);
        if ((i & 1) == 0) {
            elts.resize(i + 1);
            if (!matched) {
                elts[i] = l;
            }
        }
        else {
            elts.resize(i);
        }
        return;
    }

    if (n == 0) {
        elts.insert(elts.end(), {l, h});
        return;
    }
    else if (elts[n - 1] < l) {
        if ((n & 1) == 0)
            elts.insert(elts.end(), {l, h});

        return;
    }
    else if (h < elts[0]) {
        elts.insert(elts.begin(), {l, h});
        return;
    }

    //        [       )       [       )
    //  ..z.. b ..x.. e ..z.. b ..x.. e

    auto [li, l_be] = linear_search(l);
    auto l_xe = (li & 1) == 1;
    auto l_bxe = l_be || l_xe;
    auto l_b = l_bxe && !l_xe;

    auto [hi, h_be] = linear_search(h, li);
    auto h_xe = (hi & 1) == 1;
    auto h_bxe = h_be || h_xe;
    auto h_b = h_be && !h_xe;

    if (l_bxe || h_bxe) {
        if (l_b)
            ++li;

        if (h_b)
            ++hi;

        if (l_bxe != h_bxe)
            --hi;

        elts.erase(elts.begin() + li, elts.begin() + hi);

        if (!h_bxe)
            elts[li] = h;

        if (!l_bxe)
            elts[li] = l;
    }
    else if (li == hi) {
        elts.insert(elts.begin() + hi, {l, h});
    }
    else {
        if (li + 2 < hi) {
            elts.erase(elts.begin() + li + 2, elts.begin() + hi);
        }
        elts[li] = l;
        elts[li + 1] = h;
    }
}

template <typename T>
template <typename Func>
void ics<T>::enumerate(Func&& f) const
{
    auto i = std::size_t{0};
    auto const n = elts.size();
    while (i + 1 < n) {
        f(elts[i], elts[i + 1]);
        i += 2;
    }
    if (i < n)
        f(elts[i], elts[i]);
}

template <typename T> auto ics<T>::clear() { elts.clear(); }

template <typename T> auto to_string(ics<T> const& s) -> std::string
{
    auto r = std::string{};
    auto i = std::size_t{0};
    auto const n = s.elts.size();
    while (i + 1 < n) {
        r += "[";
        r += std::to_string(s.elts[i]);
        r += ",";
        r += std::to_string(s.elts[i + 1]);
        r += ")";
        i += 2;
    }
    if (i < n) {
        r += "[";
        r += std::to_string(s.elts[i]);
        r += "...";
    }
    return r;
}