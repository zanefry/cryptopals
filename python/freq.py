#!/usr/bin/env python3
import scipy.stats as stats

english_freqs = {}
with open('../inputs/english_counts.txt') as f:
    english_counts = {}
    for pair in [l.split() for l in f.readlines()]:
        letter = pair[0].lower()
        count = int(pair[1])
        english_counts[letter] = count

    num_samples = sum(english_counts.values())
    english_freqs = {c: n / num_samples for c, n in english_counts.items()}

def score(buf: bytes) -> float:
    if not all(chr(b).isascii() for b in buf):
        return 1000
    if not all(chr(b).isprintable() or chr(b).isspace() for b in buf):
        return 1000

    letters = [chr(b).lower() for b in buf if chr(b).isalpha()]

    counts = {c: 0 for c in english_freqs}
    for c in letters:
        counts[c] += 1

    observed = [n / len(letters) for n in list(counts.values())]
    expected = list(english_freqs.values())

    return stats.chisquare(f_obs=observed, f_exp=expected).statistic
