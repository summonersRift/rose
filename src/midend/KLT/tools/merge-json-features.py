#!/usr/bin/python

import json
import sys
from pprint import pprint

def findTileByID(tiles, ID):
	for tile in tiles:
		if tile["id"] == ID:
			return tile
	assert False

def traverse(looptree, tiles):
	if isinstance(looptree, list):
		for child in looptree:
			traverse(child, tiles)
	else:
		if looptree["type"] == "tile":
			tile = findTileByID(tiles, looptree["id"])
			looptree["length"] = tile["length"]
			looptree["stride"] = tile["stride"]
			looptree["param"] = 0

			traverse(looptree["next"], tiles)

			del looptree["id"]
			del looptree["kind"]
			del looptree["order"]
			del looptree["loop->id"]
			del looptree["tile_id"]
			del looptree["param"]
		elif looptree["type"] == "stmt":
			del looptree["code"]
		else:
			assert False
		del looptree["type"]

looptree__=sys.argv[1]
loop_ctx__=sys.argv[2]
output_=sys.argv[3]

with open(looptree__) as looptree_:
    looptree = json.load(looptree_)

with open(loop_ctx__) as loop_ctx_:
    loop_ctx = json.load(loop_ctx_)

tiles=loop_ctx["tiles"]

assert isinstance(tiles, list)

traverse(looptree, tiles)

with open(output_, 'w') as output:
    json.dump(looptree, output)

