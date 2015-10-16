#!/usr/bin/python

import json
import sys
from pprint import pprint

#sys.stdout.write('.')

def findTileByID(tiles, ID):
	for tile in tiles:
		if tile["id"] == ID:
			return tile
	assert False

def traverse(tiletree, node_cnt):
	node_id = node_cnt
	node_cnt += 1
	if isinstance(tiletree, list):
		sys.stdout.write("  node_" + str(node_id) + "[shape=\"point\"];\n")
		for child in tiletree:
			(child_id, node_cnt) = traverse(child, node_cnt)
			sys.stdout.write("    node_" + str(node_id) + " -> node_" + str(child_id) + ";\n")
	elif "next" in tiletree:
		sys.stdout.write("  node_" + str(node_id) + "[label=\"(")
		sys.stdout.write(str(tiletree["loop"]   ) + ",")
		sys.stdout.write(str(tiletree["kind"]   ) + ",")
		sys.stdout.write(str(tiletree["length"] ) + ",")
		sys.stdout.write(str(tiletree["stride"] )      )
		sys.stdout.write(")\"];\n")

		(child_id, node_cnt) = traverse(tiletree["next"], node_cnt)

		sys.stdout.write("    node_" + str(node_id) + " -> node_" + str(child_id) + ";\n")
	else:
		sys.stdout.write("  node_" + str(node_id) + "[label=\"")
		for (k,v) in tiletree["node_histogram"].iteritems():
			sys.stdout.write("(" + str(k) + "," + str(v) + "),")
		sys.stdout.write("\\n[ ")
		for i in range(0, 5):
			if str(i) in tiletree["read_by_dimension"]:
				sys.stdout.write(str(tiletree["read_by_dimension"][str(i)]) + " ")
			else:
				sys.stdout.write("0 ")
		sys.stdout.write("]\\n[ ")
		for i in range(0, 5):
			if str(i) in tiletree["write_by_dimension"]:
				sys.stdout.write(str(tiletree["write_by_dimension"][str(i)]) + " ")
			else:
				sys.stdout.write("0 ")
		sys.stdout.write("]\\n")
		for (k,v) in tiletree["operation_histogram"].iteritems():
			sys.stdout.write("(" + str(k) + "," + str(v) + "),")
		sys.stdout.write("\\n")
		for (k,v) in tiletree["index_histogram"].iteritems():
			sys.stdout.write("(" + str(k) + "," + str(v) + "),")
		sys.stdout.write("\\n")
		sys.stdout.write("\"];\n")
	return (node_id, node_cnt)

tiletree=sys.argv[1]

with open(sys.argv[1]) as tiletree_:
    tiletree = json.load(tiletree_)

print "digraph G {"
traverse(tiletree, 0)
print "}"

