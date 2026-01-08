with open("optimal_scores_and_solutions_onebased.txt", 'r') as fh:
    lines = fh.readlines()

outlines = []

for line in lines:
    linesplit = line.split()
    assert len(linesplit) > 2
    outline = linesplit[0] + " " + linesplit[1]
    for i in range(2, len(linesplit)) :
        curval = int(linesplit[i])
        outline += " " + str(curval - 1)
    print(outline)
    outlines.append(outline)