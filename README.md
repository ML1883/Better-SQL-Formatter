# Better SQL Formatter

## Description
This is a CLI program that takes a .sql or .txt file and outputs a new file with the formatted version of the SQL code. It is my first C project that I undertook with the purpose of getting a better understanding of C while making something semi-useful. I quickly realized that making a SQL formatter is quite complex, and so it became something that I just "wanted to finish". The code quality is on that level; not good.

I decided to open-source it anyway in case anyone finds my specific style of SQL code formatting agreeable; and I have not found a single formatter that emulates my style perfectly.

It does not have any customization features (i.e. enabling the control of how many spaces you use and such, though you can just change the code). It is simply unformatted file goes, formatted file comes out. It also has the -v option to print out the tokens it reads in.

## Installation
```bash
# Clone the repository
git clone https://github.com/ML1883/better-sql-formatter.git

# Navigate to the directory
cd better-sql-formatter

# Compile using the provided Makefile
make
```
Ignore the plethora of warnings and then use it in the following way:


## Usage
Note: the file needs to be in the same directory as the compiled program to work.
```bash
./main [-v] "file.sql"
```
or
```bash
./main [-v] "file.txt"
```
The -v flag shows the tokenization

## Known Issues
- Memory leaking like a sieve
- Removal of the seperating comma when making consecutive CTEs
- Probably some other stuff that could eat (i.e. remove without raising an error) your SQL code if you aren't careful. 

## Goals of formatting
The goal of this formatter is to turn this code:
```sql
WITH recursive_dates AS (
    SELECT DATE '2024-01-01' AS dt
    UNION ALL
    SELECT dt + INTERVAL '1 day' FROM recursive_dates WHERE dt < DATE '2024-01-10'
),
aggregated_sales AS (
    SELECT 
        c.customer_id,
        c.name AS customer_name,
        SUM(o.total_amount, 0) AS total_spent,
        COUNT(o.order_id) AS order_count
    FROM customers c
    LEFT JOIN orders o ON c.customer_id = o.customer_id
    WHERE o.order_date BETWEEN '2024-01-01' AND '2024-12-31'
    GROUP BY c.customer_id, c.name
    HAVING SUM(o.total_amount) > 500
)
SELECT 
    c.customer_id,
    c.name AS customer_name,
    COALESCE(a.total_spent, 0) AS total_spent,
    COALESCE(a.order_count, 0) AS order_count,
    CASE 
        WHEN a.total_spent > 1000 THEN 'VIP'
        ELSE 'Regular'
    END AS customer_status,
    (SELECT wat FROM orders WHERE customer_id = c.customer_id) AS total_orders

FROM (SELECT wat FROM orders WHERE customer_id = c.customer_id) c
LEFT JOIN aggregated_sales a ON c.customer_id = a.customer_id
WHERE EXISTS (
    SELECT 1 FROM orders o WHERE o.customer_id = c.customer_id AND o.total_amount > 100
)
ORDER BY total_spent DESC, customer_name ASC
LIMIT 50
OFFSET 10;

```

Into this code:
```sql
WITH recursive_dates AS (

  SELECT 
    DATE '2024-01-01' AS dt
  
  UNION ALL
    
  SELECT dt + INTERVAL '1 day' 
  
  FROM 
    recursive_dates 
    
  WHERE 
    dt < DATE '2024-01-10'

), aggregated_sales AS (

  SELECT 
    c.customer_id
    ,c.name AS customer_name
    ,SUM(o.total_amount, 0) AS total_spent
    ,COUNT(o.order_id) AS order_count
  
  FROM 
    customers as c
  
    LEFT JOIN as orders o 
      ON c.customer_id = o.customer_id
    
  WHERE 
    o.order_date BETWEEN '2024-01-01' AND '2024-12-31'

  GROUP BY 
    c.customer_id
    ,c.name
    
  HAVING 
    SUM(o.total_amount) > 500
)


SELECT
    c.customer_id
    ,c.name AS customer_name
    ,COALESCE(a.total_spent, 0) AS total_spent
    ,COALESCE(a.order_count, 0) AS order_count
    ,CASE 
        WHEN a.total_spent > 1000 THEN 'VIP'
        ELSE 'Regular'
        END AS customer_status
    ,(SELECT wat FROM orders WHERE customer_id = c.customer_id) AS total_orders

FROM 
  (
    SELECT 
      wat 
    
    FROM 
      orders 
      
    WHERE 
      customer_id = c.customer_id ) as c

    LEFT JOIN aggregated_sales a 
      ON c.customer_id = a.customer_id

WHERE 
  EXISTS (
    SELECT 1 FROM orders as o WHERE o.customer_id = c.customer_id AND o.total_amount > 100
    )

ORDER BY 
  total_spent DESC
  ,customer_name ASC

LIMIT 50
OFFSET 10;

```

But the current result (at time of creating this repo) is this:
```sql

WITH recursive_dates AS (
     
    
    SELECT
         DATE '2024-01-01' AS dt
    
    
    UNION ALL 
    
    SELECT
         dt +
        ,INTERVAL '1 day'
    
    FROM
        recursive_dates 
    
    WHERE
        dt < DATE '2024-01-10' 
    
    
) aggregated_sales AS (
     
    
    SELECT
         c.customer_id
        ,c.name AS customer_name
        ,SUM(o.total_amount,  0) AS total_spent
        ,COUNT(o.order_id)  AS order_count
    
    FROM
        customers c 
    
        LEFT JOIN orders o 
            ON c.customer_id = o.customer_id 
    
    WHERE
        o.order_date BETWEEN '2024-01-01' 
        AND '2024-12-31' 
    
    GROUP BY
         c.customer_id
        ,c.name
    
    HAVING
         SUM(o.total_amount) > 500
    
    
) 

SELECT
     c.customer_id
    ,c.name AS customer_name
    ,COALESCE(
        a.total_spent,  0
    ) AS total_spent
    ,COALESCE(
        a.order_count,  0
    ) AS order_count
    ,CASE
        WHEN a.total_spent > 1000  THEN 'VIP' 
        ELSE 'Regular' 
    END customer_status
    ,(
        SELECT  wat FROM orders WHERE customer_id = c.customer_id
    ) AS total_orders

FROM
    (
        SELECT wat FROM orders WHERE customer_id = c.customer_id
    ) c 

    LEFT JOIN aggregated_sales a 
        ON c.customer_id = a.customer_id 

WHERE
    EXISTS (
         
        
        SELECT 1
        
        FROM
            orders o 
        
        WHERE
            o.customer_id = c.customer_id 
            AND o.total_amount > 100 
    ) 

ORDER BY
     total_spent
    ,DESC
    ,customer_name
    ,ASC


LIMIT 50 
OFFSET 10; 

```

## Feedback and such
Feel free to comment and suggest improvements. Do know that maintenance is at my leisure.

## Disclaimer
No garantuees or warranties of any kind are provided.

## License
This project is licensed under AGPL-3.0. The copyright holder reserves the right to make exceptions to the AGPL-3.0 license at its discretion.