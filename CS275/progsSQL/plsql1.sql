DECLARE
  totalWeight NUMBER;
BEGIN
  select sum(qty * weight) into totalWeight
  from sp, p
  where sp.pno = p.pno;

  dbms_output.put_line(
    'Total Weight of Shipment = ' || TO_CHAR(totalWeight));
end;
/
