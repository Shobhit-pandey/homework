create table sConst
  ( sno    char(5)  primary key,
    sname  char(20) not null,
    status smallint,
    city   char(15)
  );
create table pConst
  ( pno    char(6)  primary key,
    pname  char(20) not null,
    color  char(6),
    weight smallint,
    city   char(15)
  );
create table spConst
  ( sno    char(5)  references sConst(sno) on delete cascade,
    pno    char(6)  references pConst(pno) on delete cascade,
    qty    integer  not null,
    primary key (sno, pno)
  );
