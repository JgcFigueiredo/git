#include "test-tool.h"
#include "cache.h"
#include "commit-graph.h"
#include "commit.h"
#include "config.h"
#include "object-store.h"
#include "object.h"
#include "repository.h"
#include "tree.h"

static void test_parse_commit_in_graph(const char *gitdir, const char *worktree,
				       const struct object_id *commit_oid)
{
	struct repository r;
	struct commit *c;
	struct commit_list *parent;

	/*
	 * Create a commit independent of any repository.
	 */
	c = lookup_commit(commit_oid);

	repo_init(&r, gitdir, worktree);

	if (!parse_commit_in_graph(&r, c))
		die("Couldn't parse commit");

	printf("%lu", c->date);
	for (parent = c->parents; parent; parent = parent->next)
		printf(" %s", oid_to_hex(&parent->item->object.oid));
	printf("\n");

	repo_clear(&r);
}

static void test_get_commit_tree_in_graph(const char *gitdir,
					  const char *worktree,
					  const struct object_id *commit_oid)
{
	struct repository r;
	struct commit *c;
	struct tree *tree;

	/*
	 * Create a commit independent of any repository.
	 */
	c = lookup_commit(commit_oid);

	repo_init(&r, gitdir, worktree);

	/*
	 * get_commit_tree_in_graph does not automatically parse the commit, so
	 * parse it first.
	 */
	if (!parse_commit_in_graph(&r, c))
		die("Couldn't parse commit");
	tree = get_commit_tree_in_graph(&r, c);
	if (!tree)
		die("Couldn't get commit tree");

	printf("%s\n", oid_to_hex(&tree->object.oid));

	repo_clear(&r);
}

int cmd__repository(int argc, const char **argv)
{
	if (argc < 2)
		die("must have at least 2 arguments");
	if (!strcmp(argv[1], "parse_commit_in_graph")) {
		struct object_id oid;
		if (argc < 5)
			die("not enough arguments");
		if (parse_oid_hex(argv[4], &oid, &argv[4]))
			die("cannot parse oid '%s'", argv[4]);
		test_parse_commit_in_graph(argv[2], argv[3], &oid);
	} else if (!strcmp(argv[1], "get_commit_tree_in_graph")) {
		struct object_id oid;
		if (argc < 5)
			die("not enough arguments");
		if (parse_oid_hex(argv[4], &oid, &argv[4]))
			die("cannot parse oid '%s'", argv[4]);
		test_get_commit_tree_in_graph(argv[2], argv[3], &oid);
	} else {
		die("unrecognized '%s'", argv[1]);
	}
	return 0;
}
