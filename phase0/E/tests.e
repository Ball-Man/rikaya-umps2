#ifndef PHASE0_TESTS_E_
#define PHASE0_TESTS_E_

/* This test will read from the 0th tape and print the content
 * on the standard output;
 * the tape should be provided with this source(probably
 * along with a completely configured umps2 vm). If this
 * is the case, this function will print a couple of quotes
 * from Douglas Adams.
 */
extern void test_read();

/* This test will use two tapes at the same time(the second and the third tape).
 * The first of the two being a 'dictionary' containing a couple of text
 * on each block. The second one being an indexer: every non-zero byte found
 * will be used to index the first tape, printing on standard output the text
 * contained on the indexed block(EOFs are considered as EOBs here)
 */
extern void test_index();

#endif
