------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
--              PGR_apspWarshall
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
\echo --q1
SELECT * FROM pgr_apspWarshall(
        'SELECT id::INTEGER, source::INTEGER, target::INTEGER, cost FROM edge_table WHERE id < 5',
        false, false
    );

\echo --q2
