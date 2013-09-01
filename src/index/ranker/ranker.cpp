/**
 * @file ranker.cpp
 * @author Sean Massung
 */

#include "index/ranker/ranker.h"

namespace meta {
namespace index {

std::vector<std::pair<doc_id, double>> ranker::score(inverted_index & idx,
                                                     document & query) const
{
    if(query.frequencies().empty())
        idx.tokenize(query);
    std::unordered_map<doc_id, double> results;

    score_data sd{idx,
                  idx.avg_doc_length(),
                  idx.num_docs(),
                  idx.total_corpus_terms(),
                  query
    };

    for(auto & tpair: query.frequencies())
    {
        sd.t_id = tpair.first;
        sd.query_term_count = tpair.second;
        sd.idf = idx.idf(tpair.first);
        for(auto & dpair: idx.counts(tpair.first))
        {
            sd.d_id = dpair.first;
            sd.doc_term_count = dpair.second;
            sd.doc_size = idx.doc_size(dpair.first);
            results[dpair.first] += score_one(sd);
        }
    }

    using doc_pair = std::pair<doc_id, double>;
    std::vector<doc_pair> final_results{results.begin(), results.end()};
    std::sort(final_results.begin(), final_results.end(),
        [](const doc_pair & a, const doc_pair & b) {
            return a.second > b.second;
        }
    );

    return final_results;
}

}
}
