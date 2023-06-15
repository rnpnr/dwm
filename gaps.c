/* Settings */
static int enablegaps = 1;

static void
setgaps(int oh, int ov, int ih, int iv)
{
	if (oh < 0) oh = 0;
	if (ov < 0) ov = 0;
	if (ih < 0) ih = 0;
	if (iv < 0) iv = 0;

	selmon->gappoh = oh;
	selmon->gappov = ov;
	selmon->gappih = ih;
	selmon->gappiv = iv;
	arrange(selmon);
}

static void
togglegaps(const Arg *arg)
{
	enablegaps = !enablegaps;
	arrange(NULL);
}

static void
defaultgaps(const Arg *arg)
{
	setgaps(gappoh, gappov, gappih, gappiv);
}

static void
incrgaps(const Arg *arg)
{
	setgaps(selmon->gappoh + arg->i,
	        selmon->gappov + arg->i,
	        selmon->gappih + arg->i,
	        selmon->gappiv + arg->i);
}

static void
incrigaps(const Arg *arg)
{
	setgaps(selmon->gappoh,
	        selmon->gappov,
	        selmon->gappih + arg->i,
	        selmon->gappiv + arg->i);
}

static void
incrogaps(const Arg *arg)
{
	setgaps(selmon->gappoh + arg->i,
	        selmon->gappov + arg->i,
	        selmon->gappih,
	        selmon->gappiv);
}

static void
getgaps(Monitor *m, int *oh, int *ov, int *ih, int *iv, unsigned int *nc)
{
	unsigned int n, oe, ie;
	oe = ie = enablegaps;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (smartgaps && n == 1) {
		oe = 0; /* outer gaps disabled when only one client */
	}

	*oh = m->gappoh*oe; /* outer horizontal gap */
	*ov = m->gappov*oe; /* outer vertical gap */
	*ih = m->gappih*ie; /* inner horizontal gap */
	*iv = m->gappiv*ie; /* inner vertical gap */
	*nc = n;            /* number of clients */
}

static void
getfacts(Monitor *m, int msize, int ssize, float *mf, float *sf, int *mr, int *sr)
{
	unsigned int n;
	float mfacts, sfacts;
	int mtotal = 0, stotal = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	mfacts = MIN(n, m->nmaster);
	sfacts = n - m->nmaster;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (n < m->nmaster)
			mtotal += msize / mfacts;
		else
			stotal += ssize / sfacts;

	*mf = mfacts; /* total factor of master area */
	*sf = sfacts; /* total factor of stack area */
	*mr = msize - mtotal; /* the remainder (rest) of pixels after an even master split */
	*sr = ssize - stotal; /* the remainder (rest) of pixels after an even stack split */
}
