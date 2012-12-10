drop table sspp;
drop table ss;
drop table pp;

create table ss (
    ss_id   int primary key auto_increment,
    sno     char(5)  not null,
    sname   char(20) not null,
    status  smallint,
    city    char(15)
  );

create table pp (
    pp_id   int primary key auto_increment,
    pno     char(6)  not null,
    pname   char(20) not null,
    color   char(6),
    weight  smallint,
    city    char(15)
  ) ENGINE=INNODB;

// foreign keys cannot be created in this way
create table sspp (
    sspp_id  int primary key auto_increment,
    ss_id    int  not null references ss,
    pp_id    int  not null references pp,
    qty      integer  not null
) ENGINE=INNODB;
