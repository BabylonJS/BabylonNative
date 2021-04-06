# Conventions for Pull Requests

At present, the Babylon Native repository predominantly relies on an 
"honor system" when it comes to pull requests, placing the bulk of the 
responsibility for making sure the PR process remains effective and 
efficient overwhelmingly in the hands of those submitting and performing 
the code reviews. Consequently, this repo does not currently have strong 
established practices for how pull requests, but we do have a number of 
conventions which we typically try to follow when performing and receiving 
code reviews. This document describes these conventions; it is formatted as 
a series of  definitions for various actions and intended usages in a 
typical Babylon  Native PR.

## All PRs Require At Least One Approval to be Checked In

Typically a single approval is sufficient, though contributors checking in
large or complex PRs may wish to gather opinions from several reviewers.
It is the responsibility of the PR's contributor to seek out reviewers who 
are knowledgeable about the part of Babylon Native to which the PR will 
contribute. Contributors are also expected to exercise discipline when 
checking in even if they have approval, making sure that the appropriate
reviewers have had an opportunity to review and that all relevant comments
have been addressed.

### A Guide to Interpreting Review Results

The "review results" -- the summarizing labels code reviewers put on their 
reviews as the last step of submitting them -- are in some cases confusingly
named relative to what they do. Usage of these results in the Babylon Native
repository is more closely associated with their effects than with their 
names, so the following descriptions serve as a guide for understanding how 
these results are likely to be interpreted in Babylon Native code reviews.

- **Approved**: This result authorizes the PR contributor to check in 
    without further review; consequently, an "Approved" code review is 
    tantamount to the statement, "This reviewer does not need to look at 
    this review again in order to feel comfortable about the change in 
    question being checked in." Note that this is *not* necessarily a 
    statement that the the code *as it appears at time of approval* is
    what the reviewer expects to be checked in; a relatively common practice
    to approve a PR while still having a small number of outstanding 
    comments. When this happens, it is still expected that the outstanding
    comments will be addressed before submission. The approval, then, can be
    interpreted as a statement that either the requested changes are small
    enough or the contributor is trusted enough (or both) that the reviewer
    is comfortable with the changes being checked in as soon as the comments
    are addressed -- without needing to wait for the reviewer to come back 
    and review the changes again.
- **Comment**: This is the most confusingly named of the review results 
    relative to its function; better names might be "Changes Suggested" or
    "Changes Recommended." A "Comment" review expresses that the 
    commented-on aspects of the submitted code may be worthy of change, or 
    at least further discussion. This review result does not express any 
    strong statement about the review, neither approving nor formally 
    blocking its submission. It is nevertheless expected that comments 
    submitted in a "Comment" review should be addressed before the code is
    checked in, even if another reviewer submits an "Approved" review at the
    same time. It is not, however, necessary or expected that every 
    "Comment" reviewer will return to the PR and submit new reviews until
    eventually approving. In fact, one common reason for submitting a 
    "Comment" review is if the reviewer feels that some other reviewer 
    should be the one to give the deciding approval, for example if the code
    being contributed to is "owned" by another frequent contributor who 
    "should" have the final say-so over what gets checked in to this code 
    area. For this and related reasons, it is expected and acceptable for 
    PRs to be checked in with few (one or more) "Approved" reviews and 
    several more "Comment" reviews, so long as all comments from all 
    reviews have been addressed.
- **Changes Requested**: This is another confusingly named review result; a
    better name for it might be, "Changes Required." This result is 
    the opposite of an approval and effectively blocks a PR from being 
    submitted until *this reviewer* returns and changes the review result to
    at least "Comment" or, ideally, "Approved." This is the strongest 
    statement a reviewer can make about a PR, and as such it is also the 
    rarest. A "Changes Requested" review result is tantamount to the 
    statement, "I see a problem with the submitted code which is dangerous
    enough that I will not be comfortable with the submission of this code, 
    no matter how many other reviewers may approve of it, unless *I 
    personally* return and see that the problem has been addressed."

To summarize, given the meanings of the various review results described 
above, all of the following statements apply:

- All comments from all code reviews **are expected to be addressed** 
    before a PR is checked in.
- A PR **may be checked in** if and only if it has at least one "Approved" 
    review result.
- A PR **is not blocked from being checked in** by any number of "Comment" 
    review results.
- A PR **cannot be checked in** with so much as a single "Changes 
    Requested" review result.

## Comments Should Be Resolved by Reviewers, Not Contributors

When discussing the handling of comments elsewhere in this document, care
has been taken to avoid the word *resolve* in favor of the word *address*.
This is because, in GitHub's terminology, *resolving* a comment is a 
specific action within the interface, and the Babylon Native convention is 
that the *resolve* action **should be performed by reviewers, not 
contributors**. Specifically, any given comment should, in general, only be
resolved by the reviewer who left that comment; this resolution signifies 
that the reviewer is satisfied that the issue described by the comment 
either no longer applies or has already been acceptably addressed. Note 
that resolving comments is the responsibility as well as the prerogative 
of the reviewer, and contributors are **not** required to wait for comments
to be resolved before the code can be checked in. All review comments must 
be *addressed*, not *resolved*, before it is acceptable to check in. It is,
for lack of a better word, polite for contributors to give reviewers a 
reasonable opportunity to resolve their comments before the PR is checked 
in; reviewers sometimes reply to comments or react to them using specific
"emojis" in order to signal to reviewers that a comment has been addressed
and may likely be resolvable. Nevertheless, it is in no way required for 
contributors to wait for all comments to be *resolved* so long as the 
comments have been *addressed*, **which is a judgement call on the part of 
the contributor**. This is important because it is expected that some 
reviewers may never return to a review in order to resolve all their 
comments. In particular, in the case of reviewers who leave an "Approved" 
review which contains unresolved comments, their already-given approval 
signals an intent to not return to this PR again.

## `nit` Comments

While, as stated above, *addressing* a comment is always to some degree a 
judgement call on the part of the contributor, sometimes a reviewer may 
wish to call out that a certain comment is specifically "optional"; the most
common example pertains to pseudo-stylistic comments concerning the look of
the code without meaningfully impacting clarity or behavior. The 
conventional way to designate such a comment is to prefix it with the `nit`
keyword, derived from the word *nitpick*.

> nit: This comment is relatively unimportant.

## PRs Should Be Squash-Merged When Checked In

This convention simply aims to help keep the Git history comprehensible and
(relatively) easy to navigate: when checking in a PR, contributors should
always squash-merge it so that all commits for that PR are "squashed" into
a single commit per PR.
