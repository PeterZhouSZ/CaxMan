#include "checks.h"

int annotate_boundaries(IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann)
{
    Vertex *v, *w;
    Edge *e;
    Triangle *t, *s;
    Node *n;

    int n_t = tin.selectBoundaryTriangles();

    if (n_t == 0)
        return n_t;

    int tid = 0;

    for (n = tin.T.head(), t = (n)?((Triangle *)n->data):NULL; n != NULL; n=n->next(), t = (n)?((Triangle *)n->data):NULL)
    {
        if (IS_BIT (t->v1(), 5) || IS_BIT (t->v2(), 5) || IS_BIT (t->v3(), 5))
        {
            caxlib::TriangleAnnotations *tid_ann = &lt_ann.at(tid);
            tid_ann->bad_geometry = true;
        }

        tid++;
    }

    // deselect boundaries
    for (n = tin.V.head(), v = (n)?((Vertex *)n->data):NULL; n != NULL; n=n->next(), v = (n)?((Vertex *)n->data):NULL)
        if (IS_BIT(v, 5))
        {
            //n_boundaries++;
            for (w = v; IS_BIT(w, 5); w = w->nextOnBoundary()) UNMARK_BIT(w, 5);
        }

    return n_t;

    //n_handles = (E.numels() - V.numels() - T.numels() + 2 * n_shells - n_boundaries) / 2;
    //d_boundaries = d_handles = d_shells = 0;
}

int annotate_no_manifold (IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann)
{
    Edge *e, *e2;
    Node *n, *m;

    List *ve, *vt;
    Triangle *t;

    int counter = 0;

    for (n = tin.E.head(), e = (n)?((Edge *)n->data):NULL; n != NULL; n=n->next(), e = (n)?((Edge *)n->data):NULL)
    {
        bool is_manifold = true;

        ve = e->v1->VE();

//        for (m = ve->head(), e2 = (m)?((Edge *)m->data):NULL; m != NULL; m=m->next(), e2 = (m)?((Edge *)m->data):NULL)
//        {
//            if (e2 != e && e2->oppositeVertex(e->v1) == e->v2)
//                std::cout << "checkConnectivity: detected duplicate edge!" << std::endl;
//        }

        if (ve->containsNode(e) == NULL)
        {
            std::cout <<  "checkConnectivity: detected non manifold vertex!" << std::endl;

            vt = e->v1->VT();

            for (m = vt->head(), t = (m)?((Triangle *)m->data):NULL; m != NULL; m=m->next(), t = (m)?((Triangle *)m->data):NULL)
                MARK_BIT (t, 5);

            is_manifold = false;

            delete vt;
        }

        delete(ve);

        if (!is_manifold)
            continue;

        ve = e->v2->VE();

//        for (m = ve->head(), e2 = (m)?((Edge *)m->data):NULL; m != NULL; m=m->next(), e2 = (m)?((Edge *)m->data):NULL)
//        {
//            if (e2 != e && e2->oppositeVertex(e->v2) == e->v1)
//            {
//                std::cout << "checkConnectivity: detected duplicate edge!" << std::endl;
//            }
//        }

        if (ve->containsNode(e) == NULL)
        {
            std::cout << "checkConnectivity: detected non manifold vertex!" << std::endl;

            vt = e->v1->VT();

            for (m = vt->head(), t = (m)?((Triangle *)m->data):NULL; m != NULL; m=m->next(), t = (m)?((Triangle *)m->data):NULL)
                MARK_BIT (t, 5);

            is_manifold = false;

            delete vt;
        };

        delete(ve);
     }

    int tid = 0;

    for (n = tin.T.head(), t = (n)?((Triangle *)n->data):NULL; n != NULL; n=n->next(), t = (n)?((Triangle *)n->data):NULL)
    {
        if (IS_BIT (t, 5))
        {
            caxlib::TriangleAnnotations *tid_ann = &lt_ann.at(tid);
            tid_ann->bad_geometry = true;

            UNMARK_BIT(t, 5);

            counter++;
        }

        tid++;
    }

    return counter;
}

int annotate_intersections (IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann)
{
    Node *n;
    Triangle *t;
    Vertex *v, *w;

    int n_t = tin.selectIntersectingTriangles();

    if (n_t == 0)
        return n_t;

    int tid = 0;

    for (n = tin.T.head(), t = (n)?((Triangle *)n->data):NULL; n != NULL; n=n->next(), t = (n)?((Triangle *)n->data):NULL)
    {
        if (IS_BIT (t->v1(), 5) || IS_BIT (t->v2(), 5) || IS_BIT (t->v3(), 5))
        {
            caxlib::TriangleAnnotations *tid_ann = &lt_ann.at(tid);
            tid_ann->bad_geometry = true;
        }

        tid++;
    }

    // deselect boundaries
    for (n = tin.V.head(), v = (n)?((Vertex *)n->data):NULL; n != NULL; n=n->next(), v = (n)?((Vertex *)n->data):NULL)
        if (IS_BIT(v, 5))
        {
            //n_boundaries++;
            for (w = v; IS_BIT(w, 5); w = w->nextOnBoundary()) UNMARK_BIT(w, 5);
        }

    return n_t;
}

int annotate_inconsistent_normals (IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann)
{
    Node *n;
    Edge *e;
    Triangle *t, *t0, *t1, *t2, *t3;

    int tid = 0;
    int tmp1, tmp2;

    int counter = 0;

    for (n = tin.T.head(), t0 = (n)?((Triangle *)n->data):NULL; n != NULL; n=n->next(), t0 = (n)?((Triangle *)n->data):NULL)
    {
        if (!IS_BIT(t0, 5))
        {
            List todo, elist;
            int isclosed = 1;

            todo.appendHead(t0);

            while (todo.numels())
            {
                t = (Triangle *)todo.head()->data;
                todo.removeCell(todo.head());
                if (!IS_BIT(t, 5))
                {
                    t1 = t->t1(); t2 = t->t2(); t3 = t->t3();
                    if (!IS_BIT(t->e1, 5)) { MARK_BIT(t->e1, 5); elist.appendHead(t->e1); }
                    if (!IS_BIT(t->e2, 5)) { MARK_BIT(t->e2, 5); elist.appendHead(t->e2); }
                    if (!IS_BIT(t->e3, 5)) { MARK_BIT(t->e3, 5); elist.appendHead(t->e3); }

                    if (t1 != NULL && !IS_BIT(t1, 5)) { todo.appendHead(t1); if (!t->checkAdjNor(t1)) { t1->invert(); } }
                    if (t2 != NULL && !IS_BIT(t2, 5)) { todo.appendHead(t2); if (!t->checkAdjNor(t2)) { t2->invert(); } }
                    if (t3 != NULL && !IS_BIT(t3, 5)) { todo.appendHead(t3); if (!t->checkAdjNor(t3)) { t3->invert(); } }

                    MARK_BIT(t, 5);
                }
            }

            FOREACHVEEDGE((&(elist)), e, n)
            {
                UNMARK_BIT(e, 5);
                if (isclosed && e->isOnBoundary()) isclosed = 0;
                tmp1 = (e->t1 != NULL)?((e->commonVertex(e->t1->nextEdge(e)) == e->v1)?(-1):(1)):(0);
                tmp2 = (e->t2 != NULL)?((e->commonVertex(e->t2->nextEdge(e)) == e->v2)?(-1):(1)):(0);

                if (tmp1*tmp2 < 0)
                {
                    caxlib::TriangleAnnotations *tid_ann = &lt_ann.at(tid);
                    tid_ann->bad_geometry = true;
                    counter++;
                }
            }

        }

        tid++;

    }

    for (n = tin.T.head(), t = (n)?((Triangle *)n->data):NULL; n != NULL; n=n->next(), t = (n)?((Triangle *)n->data):NULL)
        UNMARK_BIT(t, 5);

    return counter;
}
