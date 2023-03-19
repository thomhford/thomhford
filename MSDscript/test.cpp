//
// Created by Thomas Ford on 1/24/23.
//

#include "Expr.h"
#include "parse.h"
#include "catch.h"
#include "Val.h"
#include <iostream>

// Test Cases:
TEST_CASE( "equals" ){
    SECTION( "num" )
    {
        CHECK((new NumExpr(10))->equals(new NumExpr(10)) == true);
        CHECK((new NumExpr(8))->equals(new NumExpr(10)) == false);
        CHECK((new NumExpr(-8))->equals(new NumExpr(8)) == false);
        CHECK((new NumExpr(-5))->equals(new NumExpr(-5)) == true);
        CHECK((new NumExpr(5))->equals(new AddExpr(new NumExpr(2), new NumExpr(3))) == false);
        CHECK((new NumExpr(5))->equals(new MultExpr(new NumExpr(2), new NumExpr(3))) == false);
        CHECK((new NumExpr(5))->equals(new VarExpr("x")) == false);
    }

    SECTION( "add" )
    {
        CHECK((new AddExpr(new NumExpr(3), new NumExpr(5))) -> equals(
                new AddExpr(new NumExpr(3),new NumExpr(5))) == true);
        CHECK((new AddExpr(new NumExpr(1), new NumExpr(2))) -> equals(
                new AddExpr(new NumExpr(1),new NumExpr(4))) == false);
        CHECK((new AddExpr(new NumExpr(1), new NumExpr(2))) -> equals(
                new MultExpr(new NumExpr(1),new NumExpr(4))) == false);
        CHECK((new AddExpr(new NumExpr(1), new NumExpr(2))) -> equals(
                new VarExpr("x")) == false);
        CHECK((new AddExpr(new NumExpr(1), new NumExpr(2))) -> equals(
                new NumExpr(4)) == false);
    }

    SECTION( "mult" )
    {
        CHECK((new MultExpr(new NumExpr(3), new NumExpr(5))) -> equals(
                new MultExpr(new NumExpr(3),new NumExpr(5))) == true);
        CHECK((new MultExpr(new NumExpr(1), new NumExpr(2))) -> equals(
                new MultExpr(new NumExpr(1),new NumExpr(4))) == false);
        CHECK((new MultExpr(new NumExpr(1), new NumExpr(2))) -> equals(
                new AddExpr(new NumExpr(1),new NumExpr(2))) == false);
    }

    SECTION( "variable" )
    {
        CHECK((new VarExpr("x"))->equals(new VarExpr("x")) == true);
        CHECK((new VarExpr("x"))->equals(new VarExpr("y")) == false);
        CHECK((new NumExpr(-8))->equals(new VarExpr("x")) == false);
        CHECK((new VarExpr(""))->equals(new VarExpr("x")) == false);
        CHECK((new VarExpr(""))->equals(new VarExpr("")) == true);
        CHECK((new VarExpr("x"))->equals(new AddExpr(new NumExpr(1), new NumExpr(2)))== false);
        CHECK((new VarExpr("x"))->equals(new MultExpr(new NumExpr(1), new NumExpr(2)))== false);
    }

    SECTION( "dynamic_cast" )
    {
        auto n1 = new NumExpr(5);
        auto v1 = new VarExpr("x");
        auto a1 = new AddExpr(n1, n1);
        auto a2 = new AddExpr(n1, v1);
        auto m1 = new MultExpr(n1, n1);
        auto l1 = new LetExpr("x", n1, a2);

        CHECK(dynamic_cast<VarExpr*>(v1) != nullptr);
        CHECK(dynamic_cast<VarExpr*>(n1) == nullptr);
        CHECK(dynamic_cast<NumExpr*>(n1) != nullptr);
        CHECK(dynamic_cast<AddExpr*>(n1) == nullptr);
        CHECK(dynamic_cast<MultExpr*>(n1) == nullptr);
        CHECK(dynamic_cast<NumExpr*>(a1) == nullptr);
        CHECK(dynamic_cast<AddExpr*>(a1) != nullptr);
        CHECK(dynamic_cast<MultExpr*>(a1) == nullptr);
        CHECK(dynamic_cast<NumExpr*>(m1) == nullptr);
        CHECK(dynamic_cast<AddExpr*>(m1) == nullptr);
        CHECK(dynamic_cast<MultExpr*>(m1) != nullptr);
        CHECK(dynamic_cast<LetExpr*>(m1) == nullptr);
        CHECK(dynamic_cast<LetExpr*>(l1) != nullptr);
    }
}
TEST_CASE( "interp" ){
    SECTION( "num" )
    {
        CHECK( (new NumExpr(3))->interp()->equals(new NumVal(3)));
        CHECK( !(new NumExpr(3))->interp()->equals(new NumVal(4)));
        CHECK( (new NumExpr(0))->interp()->equals(new NumVal(0)));
    }
    SECTION( "addition" )
    {
        CHECK( (new AddExpr(new NumExpr(3), new NumExpr(2)))
                       ->interp()->equals(new NumVal(5)));
        CHECK( (new AddExpr(new AddExpr(new NumExpr(10), new NumExpr(15)),
                            new AddExpr(new NumExpr(20),new NumExpr(20))))
                       ->interp()->equals(new NumVal(65)));
        CHECK( (new AddExpr(new MultExpr(new NumExpr(2), new NumExpr(3)),
                            new AddExpr(new NumExpr(4),new NumExpr(5))))
                       ->interp()->equals(new NumVal(15)));
    }
    SECTION( "multiply" )
    {
        CHECK( (new MultExpr(new NumExpr(3), new NumExpr(2)))
                       ->interp()->equals(new NumVal(6)));
        CHECK( (new MultExpr(new MultExpr(new NumExpr(2), new NumExpr(3)),
                            new MultExpr(new NumExpr(4),new NumExpr(5))))
                       ->interp()->equals(new NumVal(120)));
        CHECK( (new MultExpr(new AddExpr(new NumExpr(2), new NumExpr(3)),
                             new AddExpr(new NumExpr(4),new NumExpr(2))))
                       ->interp()->equals(new NumVal(30)));
    }SECTION( "variable values" )
    {
        CHECK_THROWS_WITH( (new VarExpr("x"))->interp(), "variable expression has no value" );
    }
    SECTION( "has_variable" )
    {
        CHECK( !(new NumExpr(0))->has_variable()); // Should be false. Int is not variable
        CHECK( (new MultExpr(new NumExpr(3), new VarExpr("x")))->has_variable() );
        CHECK( (new AddExpr(new NumExpr(3), new VarExpr("x")))->has_variable() );
        CHECK( (new VarExpr("x"))->has_variable() );
    }
}

TEST_CASE( "subst" ) {
    SECTION( "yes subst" )
    {
        CHECK( (new AddExpr(new VarExpr("x"), new NumExpr(7)))
                        ->subst("x", new VarExpr("y"))
                        ->equals(new AddExpr(new VarExpr("y"), new NumExpr(7))));
        CHECK( (new VarExpr("x"))
                        ->subst("x", new AddExpr(new VarExpr("y"),new NumExpr(7)))
                        ->equals(new AddExpr(new VarExpr("y"),new NumExpr(7))));
        CHECK( (new MultExpr(new VarExpr("x"), new NumExpr(2)))->subst("x", new NumExpr(3))
                       ->equals(new MultExpr(new NumExpr(3), new NumExpr(2))));

    }
    SECTION( "no subst" )
    {
        CHECK( (new NumExpr(7))->subst("x", new VarExpr("y"))->equals(new NumExpr(7))==true );
        CHECK( (new VarExpr("x"))->subst("W", new VarExpr("y"))->equals(new VarExpr("x"))==true );
        CHECK( (new AddExpr(new NumExpr(8), new NumExpr(2)))->subst("x", new VarExpr("y"))
                       ->equals(new AddExpr(new NumExpr(8), new NumExpr(2)))==true );
        CHECK( (new MultExpr(new NumExpr(1), new NumExpr(2)))->subst("z",new NumExpr(3))
                       ->equals(new MultExpr(new NumExpr(1), new NumExpr(2))));
    }
}

TEST_CASE( "print" ){
    SECTION( "num" )
    {
        CHECK( (new NumExpr(10))->to_string() == "10" );
    }
    SECTION( "var" )
    {
        CHECK( (new VarExpr("x"))->to_string() == "x");
    }
    SECTION( "add" )
    {
        CHECK( (new AddExpr(new NumExpr(10), new NumExpr(10)))->to_string() == "(10+10)");
        CHECK( (new AddExpr( new AddExpr(new NumExpr(10), new NumExpr(10)),
                             new NumExpr(10)))->to_string() == "((10+10)+10)");
        CHECK( (new AddExpr( new AddExpr(new NumExpr(10), new NumExpr(10)), new AddExpr(
                             new NumExpr(10), new NumExpr(10))))->to_string() == "((10+10)+(10+10))");
        CHECK( (new AddExpr( new AddExpr(new AddExpr(new NumExpr(10), new NumExpr(10)),
                                         new NumExpr(10)), new NumExpr(10)))->to_string()
                                         == "(((10+10)+10)+10)");
    }
    SECTION( "mult" )
    {
        CHECK( (new MultExpr(new NumExpr(10), new NumExpr(10)))->to_string() == "(10*10)");
        CHECK( (new MultExpr( new MultExpr(new NumExpr(10), new NumExpr(10)),
                             new NumExpr(10)))->to_string() == "((10*10)*10)");
        CHECK( (new MultExpr( new MultExpr(new NumExpr(10), new NumExpr(10)), new MultExpr(
                new NumExpr(10), new NumExpr(10))))->to_string() == "((10*10)*(10*10))");
        CHECK( (new MultExpr( new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)),
                                         new NumExpr(10)), new NumExpr(10)))->to_string()
               == "(((10*10)*10)*10)");
    }
    SECTION( "mix" )
    {
        CHECK( (new AddExpr(new MultExpr(new NumExpr(10), new VarExpr("x")),
                            new NumExpr(10)))->to_string() == "((10*x)+10)");
        CHECK( (new MultExpr(new AddExpr(new MultExpr(new NumExpr(10), new VarExpr("x")),
                                         new NumExpr(10)), new NumExpr(10)))->to_string()
                                         == "(((10*x)+10)*10)");
    }
}

TEST_CASE( "pretty_print" ){
    SECTION( "num" )
    {
        CHECK( (new NumExpr(10))->to_pretty_string() == "10" );
    }
    SECTION( "var" )
    {
        CHECK( (new VarExpr("x"))->to_pretty_string() == "x" );
    }
    SECTION( "add" )
    {
        CHECK( (new AddExpr(new NumExpr(10), new NumExpr(10)))->to_pretty_string() == "10 + 10");
        CHECK( (new AddExpr( new AddExpr(new NumExpr(10), new NumExpr(10)),
                             new NumExpr(10)))->to_pretty_string() == "(10 + 10) + 10");
        CHECK( (new AddExpr(new NumExpr(2), new AddExpr(new NumExpr(3),
                                                        new NumExpr(4))))->to_pretty_string() == "2 + 3 + 4");
        CHECK( (new AddExpr( new AddExpr(new NumExpr(10), new NumExpr(10)), new AddExpr(
                new NumExpr(10), new NumExpr(10))))->to_pretty_string() == "(10 + 10) + 10 + 10");
        CHECK( (new AddExpr(new NumExpr(1), new MultExpr(new NumExpr(2),
                                                         new NumExpr(3))))->to_pretty_string() == "1 + 2 * 3");
        CHECK( (new AddExpr(new AddExpr(new AddExpr(new NumExpr(10),
                                                       new NumExpr(10)),new NumExpr(10)),
                             new NumExpr(10)))->to_pretty_string() == "((10 + 10) + 10) + 10");
    }
    SECTION( "mult" )
    {
        CHECK( (new MultExpr(new NumExpr(10), new NumExpr(10)))->to_pretty_string() == "10 * 10");
        CHECK( (new MultExpr( new MultExpr(new NumExpr(10), new NumExpr(10)),
                              new NumExpr(10)))->to_pretty_string() == "(10 * 10) * 10");
        CHECK( (new MultExpr( new MultExpr(new NumExpr(10), new NumExpr(10)), new MultExpr(
                new NumExpr(10), new NumExpr(10))))->to_pretty_string() == "(10 * 10) * 10 * 10");
        CHECK( (new MultExpr(new MultExpr(new NumExpr(2), new NumExpr(3)),
                             new NumExpr(4)))->to_pretty_string() == "(2 * 3) * 4");
        CHECK( (new MultExpr(new NumExpr(2), new MultExpr(
                new NumExpr(3),new NumExpr(4))))->to_pretty_string() == "2 * 3 * 4");
        CHECK( (new MultExpr(new MultExpr(new MultExpr(new NumExpr(10),
                                                       new NumExpr(10)),new NumExpr(10)),
                             new NumExpr(10)))->to_pretty_string() == "((10 * 10) * 10) * 10");
        CHECK( (new MultExpr(new MultExpr(new AddExpr(new NumExpr(10),
                                                       new NumExpr(10)),new NumExpr(10)),
                             new NumExpr(10)))->to_pretty_string() == "((10 + 10) * 10) * 10");
    }
    SECTION( "mix" )
    {
        CHECK( (new AddExpr(new NumExpr(1),new MultExpr(
                new NumExpr(2),new NumExpr(3))))->to_pretty_string() == "1 + 2 * 3");
        CHECK( (new MultExpr(new NumExpr(1), new AddExpr(
                new NumExpr(2),new NumExpr(3))))->to_pretty_string() == "1 * (2 + 3)");
        CHECK( (new AddExpr(new MultExpr(new NumExpr(2), new NumExpr(3)),
                            new NumExpr(4)))->to_pretty_string() == "2 * 3 + 4");
        CHECK( (new MultExpr(new AddExpr(new MultExpr(new NumExpr(10),
                                                      new NumExpr(10)),new NumExpr(10)),
                             new NumExpr(10)))->to_pretty_string() == "(10 * 10 + 10) * 10");
        CHECK( (new MultExpr(new MultExpr(new AddExpr(new NumExpr(10),
                                                      new NumExpr(10)),new NumExpr(10)),
                             new NumExpr(10)))->to_pretty_string() == "((10 + 10) * 10) * 10");
        CHECK( (new AddExpr(new MultExpr(new NumExpr(10),
                                         new NumExpr(10)),new MultExpr(new NumExpr(10),
                                                                       new NumExpr(10))))->to_pretty_string()
                                                                       == "10 * 10 + 10 * 10");
        CHECK( (new AddExpr(new MultExpr(new MultExpr(new NumExpr(10),
                                                      new NumExpr(10)),new NumExpr(10)),
                            new NumExpr(10)))->to_pretty_string() == "(10 * 10) * 10 + 10");
        CHECK( (new AddExpr(new MultExpr(new AddExpr(new NumExpr(10),
                                                     new NumExpr(10)),new NumExpr(10)),
                            new NumExpr(10)))->to_pretty_string() == "(10 + 10) * 10 + 10");
    }
}

TEST_CASE( "let binding" ){
    SECTION( "equals" )
    {
        CHECK( (new LetExpr("x", new NumExpr(10),
                            new AddExpr(new VarExpr("x"),new NumExpr(10))))
                            ->equals((new LetExpr("x",
                                                  new NumExpr(10),new AddExpr(new VarExpr("x"),
                                                                              new NumExpr(10))))));
        CHECK( !(new LetExpr("x", new NumExpr(10),
                            new AddExpr(new VarExpr("x"),new NumExpr(10))))
                       ->equals((new LetExpr("x",
                                             new NumExpr(11),new AddExpr(new VarExpr("x"),
                                                                         new NumExpr(10))))));
        CHECK( !(new LetExpr("x", new NumExpr(10),
                             new AddExpr(new VarExpr("y"),new NumExpr(10))))
                ->equals((new LetExpr("x",
                                      new NumExpr(10),new AddExpr(new VarExpr("x"),
                                                                  new NumExpr(10))))));
        CHECK( !(new LetExpr("y", new NumExpr(10),
                             new AddExpr(new VarExpr("x"),new NumExpr(10))))
                ->equals((new LetExpr("x",
                                      new NumExpr(10),new AddExpr(new VarExpr("x"),
                                                                  new NumExpr(10))))));
        CHECK( !(new LetExpr("x", new NumExpr(1), new AddExpr(new VarExpr(
                "x"), new NumExpr(2))))->equals(new AddExpr(new VarExpr("x"), new NumExpr(2))));
        CHECK( !(new LetExpr("x", new NumExpr(1), new AddExpr(new VarExpr(
                "x"), new NumExpr(2))))->equals(new MultExpr(new VarExpr("x"), new NumExpr(2))));
        CHECK( !(new LetExpr("x", new NumExpr(1), new AddExpr(new VarExpr(
                "x"), new NumExpr(2))))->equals(new NumExpr(2)));
        CHECK( !(new LetExpr("x", new NumExpr(1), new AddExpr(new VarExpr(
                "x"), new NumExpr(2))))->equals(new VarExpr("x")));
    }
    SECTION( "has_variable" )
    {
        CHECK( (new LetExpr("x", new NumExpr(10),new AddExpr(new VarExpr("x"),
                                                             new NumExpr(10))))->has_variable());
        CHECK( !(new LetExpr("x", new NumExpr(10),new AddExpr(new NumExpr(10),
                                                             new NumExpr(10))))->has_variable());
        CHECK( (new LetExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(10)),
                            new AddExpr(new NumExpr(10),new NumExpr(10))))->has_variable());
    }
    SECTION( "interp add" )
    {
        CHECK( (new LetExpr("x", new NumExpr(10), new AddExpr(new VarExpr("x"),
                                                              new NumExpr(10))))->interp() ->equals(new NumVal(20)));
        CHECK( (new LetExpr("x", new AddExpr(new NumExpr(10), new NumExpr(10)),
                            new AddExpr(new VarExpr("x"), new NumExpr(10))))->interp() ->equals(new NumVal(30)));
        CHECK_THROWS_WITH( (new LetExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(10)),
                            new AddExpr(new VarExpr("x"),
                                        new NumExpr(10))))->interp(), "variable expression has no value" );
    }
    SECTION( "interp mult" )
    {
        CHECK( (new LetExpr("x", new NumExpr(10), new MultExpr(new VarExpr("x"),
                                                              new NumExpr(10))))->interp() ->equals(new NumVal(100)));
        CHECK( (new LetExpr("x", new MultExpr(new NumExpr(10), new NumExpr(10)),
                            new MultExpr(new VarExpr("x"), new NumExpr(10))))->interp() ->equals(new NumVal(1000)));
        CHECK_THROWS_WITH( (new LetExpr("x", new MultExpr(new VarExpr("x"), new NumExpr(10)),
                                        new MultExpr(new VarExpr("x"),
                                                    new NumExpr(10))))->interp(), "variable expression has no value" );
    }
    SECTION( "interp deep" )
    {
        CHECK((new LetExpr("x", new NumExpr(1),
                           new LetExpr("y", new NumExpr(2), new LetExpr("z",
                                                                        new NumExpr(3), new AddExpr(
                                           new AddExpr(new VarExpr("x"),
                                                       new VarExpr("y")),
                                           new VarExpr("z"))))))
                      ->interp() ->equals(new NumVal(6)));

        CHECK((new LetExpr("x", new NumExpr(1),
                           new LetExpr("x", new NumExpr(2),
                                       new LetExpr("x", new NumExpr(3), new AddExpr(
                                               new AddExpr(new VarExpr("x"), new VarExpr("x")),
                                               new VarExpr("x"))))))->interp() ->equals(new NumVal(9)));
    }

    SECTION( "print" )
    {
        CHECK( (new LetExpr("x", new NumExpr(5), new AddExpr(
                new LetExpr("y",new NumExpr(3), new AddExpr(
                        new VarExpr("y"),new NumExpr(2))),
                        new VarExpr("x"))))->to_string() == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");
    }
    SECTION( "pretty_print" )
    {
        CHECK((new LetExpr("x", new NumExpr(5), new AddExpr(
                new LetExpr("y", new NumExpr(3), new AddExpr(
                        new VarExpr("y"), new NumExpr(2))),
                new VarExpr("x"))))->to_pretty_string() == "_let x = 5\n"
                                                           "_in  (_let y = 3\n"
                                                           "      _in  y + 2) + x");
        CHECK((new AddExpr(new MultExpr(new NumExpr(5), new LetExpr(
                "x", new NumExpr(5), new VarExpr("x"))), new NumExpr(
                1)))->to_pretty_string() == "5 * (_let x = 5\n"
                                                "     _in  x) + 1");
        CHECK((new AddExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new NumExpr(
                1)))->to_pretty_string() == "(_let x = 5\n"
                                                " _in  x) + 1");
        CHECK((new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new AddExpr(
                new VarExpr("x"), new NumExpr(1)))))->to_pretty_string() == "5 * _let x = 5\n"
                                                                                              "    _in  x + 1");
        CHECK((new MultExpr(new NumExpr(5), new AddExpr(
                new LetExpr("x", new NumExpr(5), new VarExpr("x")),
                new NumExpr(1))))->to_pretty_string() == "5 * ((_let x = 5\n"
                                                                  "      _in  x) + 1)");
        CHECK((new LetExpr("x", new NumExpr(1), new LetExpr("y", new NumExpr(2), new LetExpr(
                "z", new NumExpr(3), new AddExpr(new AddExpr(new VarExpr("x"), new VarExpr(
                        "y")), new VarExpr("z"))))))->to_pretty_string() == "_let x = 1\n"
                                                                            "_in  _let y = 2\n"
                                                                            "     _in  _let z = 3\n"
                                                                            "          _in  (x + y) + z");
        CHECK((new LetExpr("x", new NumExpr(1),
                           new LetExpr("x", new NumExpr(2),
                                       new LetExpr("x", new NumExpr(3), new AddExpr(
                                               new AddExpr(new VarExpr("x"), new VarExpr("x")),
                                               new VarExpr("x"))))))->to_pretty_string() == "_let x = 1\n"
                                                                                            "_in  _let x = 2\n"
                                                                                            "     _in  _let x = 3\n"
                                                                                            "          _in  (x + x) + x");
    }

}



TEST_CASE("parse") {
    CHECK_THROWS_WITH( parse_str("()"), "invalid input" );

    CHECK( parse_str("(1)")->equals(new NumExpr(1)) );
    CHECK( parse_str("(((1)))")->equals(new NumExpr(1)) );

    CHECK_THROWS_WITH( parse_str("(1"), "missing close parenthesis" );

    CHECK( parse_str("1")->equals(new NumExpr(1)) );
    CHECK( parse_str("10")->equals(new NumExpr(10)) );
    CHECK( parse_str("-3")->equals(new NumExpr(-3)) );
    CHECK( parse_str("  \n 5  ")->equals(new NumExpr(5)) );
    CHECK_THROWS_WITH( parse_str("-"), "invalid input" );

//    // This was some temporary debugging code:
//      std::istringstream in("_let x = 2 _in x + 2");
//      parse_expr(in)->pretty_print(std::cout); std::cout << "\n";

    CHECK_THROWS_WITH( parse_str(" -   5  "), "invalid input" );

    CHECK( parse_str("x")->equals(new VarExpr("x")) );
    CHECK( parse_str("xyz")->equals(new VarExpr("xyz")) );
    CHECK( parse_str("xYz")->equals(new VarExpr("xYz")) );
    CHECK_THROWS_WITH( parse_str("x_z"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("-x"), "invalid input" );

    CHECK( parse_str("x + y")->equals(new AddExpr(new VarExpr("x"), new VarExpr("y"))) );

    CHECK( parse_str("x * y")->equals(new MultExpr(new VarExpr("x"), new VarExpr("y"))) );

    CHECK( parse_str("z * x + y")
                   ->equals(new AddExpr(new MultExpr(new VarExpr("z"), new VarExpr("x")),
                                    new VarExpr("y"))) );

    CHECK( parse_str("z * (x + y)")
                   ->equals(new MultExpr(new VarExpr("z"),
                                     new AddExpr(new VarExpr("x"), new VarExpr("y"))) ));
    CHECK( parse_str("_let x = 5 _in x + 2")
                   ->equals(new LetExpr("x", new NumExpr(5),
                                         new AddExpr(new VarExpr("x"), new NumExpr(2))) ));
    CHECK( parse_str("(_let y=4 _in y) + 5")->equals ( new AddExpr(
            new LetExpr("y",new NumExpr(4), new VarExpr("y")), new NumExpr(5))) );


    CHECK_THROWS_WITH( parse_str("_et x = 5 _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_lt x = 5 _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_le x = 5 _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let = 5 _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let 5 = 5 _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let x 5 _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let x = _in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let x = 5 in x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let x = 5 _n x + 2"), "invalid input" );
    CHECK_THROWS_WITH( parse_str("_let x = 5 _i x + 2"), "invalid input" );

    CHECK(parse_str("_let x=1 _in _let y=1 _in _let z=x+1 _in x+y*z")->interp() ->equals(new NumVal(3)));

}

TEST_CASE("values"){
    SECTION("numVal")
    {
        CHECK((new NumVal(10))->equals(new NumVal(10)) == true);
        CHECK((new NumVal(8))->equals(new NumVal(10)) == false);
        CHECK((new NumVal(-8))->equals(new NumVal(8)) == false);
        CHECK((new NumVal(-5))->equals(new NumVal(-5)) == true);
        CHECK((new NumVal(10))->to_string() == "10");
        CHECK((new NumVal(10))->to_expr()->equals(new NumExpr(10)));
        CHECK(!(new NumVal(10))->to_expr()->equals(new AddExpr(new NumExpr(5), new NumExpr(5))));
        CHECK(!(new NumVal(10))->equals(nullptr));
        CHECK_THROWS_WITH((new NumVal(10))->add_to(nullptr), "add of non-number");
        CHECK_THROWS_WITH((new NumVal(10))->add_to(new BoolVal(true)), "add of non-number");
        CHECK_THROWS_WITH((new NumVal(10))->mult_with(nullptr), "mult of non-number");
        CHECK_THROWS_WITH((new NumVal(10))->mult_with(new BoolVal(true)), "mult of non-number");
    }
    SECTION("boolVal")
    {
        CHECK((new BoolVal(true))->equals(new BoolVal(true)));
        CHECK(!(new BoolVal(true))->equals(new BoolVal(false)));
        CHECK(!(new BoolVal(true))->equals(new NumVal(10)));
        CHECK((new BoolVal(true))->to_string() == "_true");
        CHECK((new BoolVal(false))->to_string() == "_false");
        CHECK((new BoolVal(true))->to_expr()->equals(new BoolExpr(true)));
        CHECK(!(new BoolVal(false))->to_expr()->equals(new BoolExpr(true)));
        CHECK_THROWS_WITH((new BoolVal(true))->add_to(nullptr), "add of non-number");
        CHECK_THROWS_WITH((new BoolVal(true))->add_to(new BoolVal(true)), "add of non-number");
        CHECK_THROWS_WITH((new BoolVal(true))->mult_with(nullptr), "mult of non-number");
        CHECK_THROWS_WITH((new BoolVal(true))->mult_with(new BoolVal(true)), "mult of non-number");
    }
}

TEST_CASE("boolExpr"){
    SECTION("equals")
    {
        CHECK((new BoolExpr(true))->equals(new BoolExpr(true)));
        CHECK(!(new BoolExpr(true))->equals(new BoolExpr(false)));
        CHECK(!(new BoolExpr(true))->equals(nullptr));
        CHECK(!(new BoolExpr(true))->equals(new NumExpr(10)));
        CHECK(!(new BoolExpr(true))->equals(new VarExpr("d")));
        CHECK(!(new BoolExpr(true))->equals(new AddExpr(new NumExpr(5), new NumExpr(5))));
        CHECK(!(new BoolExpr(true))->equals(new MultExpr(new NumExpr(5), new NumExpr(5))));
        CHECK(!(new BoolExpr(true))->equals(new LetExpr("x",
                                                        new NumExpr(5), new AddExpr(
                                                                new NumExpr(5), new VarExpr("x")))));
    }
    SECTION("interp")
    {
        CHECK((new BoolExpr(true))->interp()->equals(new BoolVal(true)));
        CHECK(!(new BoolExpr(true))->interp()->equals(new BoolVal(false)));
    }
    SECTION("other")
    {
        CHECK(!(new BoolExpr(true))->has_variable());
        CHECK((new BoolExpr(true))->subst("x", new NumExpr(1)));
        CHECK((new BoolExpr(true))->to_string() == "_true");
        CHECK((new BoolExpr(false))->to_string() == "_false");
        CHECK((new BoolExpr(true))->to_pretty_string() == "_true");
        CHECK((new BoolExpr(false))->to_pretty_string() == "_false");
    }
}

TEST_CASE("IfExpr"){
    SECTION("equals")
    {
        CHECK((new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new NumExpr(5)))->equals(new IfExpr(new BoolExpr(true),
                                                              new NumExpr(5), new NumExpr(5))));
        CHECK(!(new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new NumExpr(5)))->equals(new IfExpr(new BoolExpr(false),
                                                              new NumExpr(5), new NumExpr(5))));
        CHECK(!(new IfExpr(new BoolExpr(true), new NumExpr(5),
                           new NumExpr(5)))->equals(new IfExpr(new BoolExpr(true),
                                                               new NumExpr(1), new NumExpr(5))));
        CHECK(!(new IfExpr(new BoolExpr(true), new NumExpr(5),
                           new NumExpr(5)))->equals(new IfExpr(new BoolExpr(true),
                                                               new NumExpr(5), new NumExpr(0))));
        CHECK(!(new IfExpr(new BoolExpr(true), new NumExpr(5),
                           new NumExpr(5)))->equals(nullptr));
    }
    SECTION("interp")
    {
        CHECK((new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new NumExpr(10)))->interp()->equals(new NumVal(5)));
        CHECK((new IfExpr(new BoolExpr(false), new NumExpr(5),
                          new NumExpr(10)))->interp()->equals(new NumVal(10)));
    }
    SECTION("subst")
    {
        CHECK((new IfExpr(new BoolExpr(true), new VarExpr("x"),
                          new NumExpr(10)))->subst("x", new NumExpr(5))->equals(
                                  (new IfExpr(new BoolExpr(true), new NumExpr(5),
                                              new NumExpr(10)))));
        CHECK((new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new VarExpr("x")))->subst("x", new NumExpr(5))->equals(
                                  (new IfExpr(new BoolExpr(true), new NumExpr(5),
                                              new NumExpr(5)))));
    }
    SECTION("other")
    {
        CHECK(!(new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new NumExpr(10)))->has_variable());
        CHECK((new IfExpr(new BoolExpr(true), new VarExpr("x"),
                           new NumExpr(10)))->has_variable());
        CHECK((new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new VarExpr("x")))->has_variable());
        CHECK((new IfExpr(new BoolExpr(true), new NumExpr(5),
                          new VarExpr("x")))->to_string() == "(_if _true _then 5 _else x)");
        CHECK_THROWS_WITH((new IfExpr(new NumExpr(5), new NumExpr(5),
                                      new VarExpr("x")))->interp(), "Not a boolean");

    }
}

TEST_CASE("EqExpr") {
    SECTION("equals"){
        CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(
                new EqExpr(new NumExpr(5), new NumExpr(5))));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(
                new EqExpr(new NumExpr(0), new NumExpr(5))));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(
                new EqExpr(new NumExpr(5), new NumExpr(0))));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(nullptr));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(new NumExpr(5)));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(new VarExpr("x")));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(
                new AddExpr(new NumExpr(4), new NumExpr(4))));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(new LetExpr("x",
                      new NumExpr(11),new AddExpr(new VarExpr("x"),
                      new NumExpr(10)))));
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(new BoolExpr(true)));
    }
    SECTION("interp")
    {
        CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->interp()->equals(new BoolVal(true)));
        CHECK((new EqExpr(new NumExpr(5), new NumExpr(6)))->interp()->equals(new BoolVal(false)));
    }
    SECTION("other")
    {
        CHECK(!(new EqExpr(new NumExpr(5), new NumExpr(5)))->has_variable());
        CHECK((new EqExpr(new VarExpr("x"), new NumExpr(5)))->has_variable());
        CHECK((new EqExpr(new NumExpr(5), new VarExpr("x")))->has_variable());
        CHECK((new EqExpr(new NumExpr(5), new VarExpr("x")))
            ->subst("x",new NumExpr(5))->equals(new EqExpr(new NumExpr(5), new NumExpr(5))));
        CHECK((new EqExpr(new NumExpr(5), new VarExpr("x")))->to_string() == "(5==x)");
    }
}

TEST_CASE("Parse Conditionals"){
    SECTION("EqExpr"){
        CHECK( parse_str("1 == 1")->equals(new EqExpr(new NumExpr(1), new NumExpr(1))));
        CHECK( parse_str("1 == 1")->interp()->equals(new BoolVal(true)));
        CHECK( parse_str("       1==1")->equals(new EqExpr(new NumExpr(1), new NumExpr(1))));
        CHECK( parse_str("1==1     ")->equals(new EqExpr(new NumExpr(1), new NumExpr(1))));
        CHECK(!(parse_str("2 == 1")->equals(new EqExpr(new NumExpr(1), new NumExpr(1)))));
        CHECK((parse_str("2 == 1")->interp()->equals(new BoolVal(false))));
        CHECK((parse_str("a == 1")->equals(new EqExpr(new VarExpr("a"), new NumExpr(1)))));
        CHECK_THROWS_WITH(( parse_str("1 = 1")), "invalid input");
        CHECK_THROWS_WITH(( parse_str("1 =! 1")), "invalid input");
        CHECK_THROWS_WITH(( parse_str("1 === 1")), "invalid input");
    }
    SECTION("IfExpr")
    {
        CHECK( parse_str("_if _true _then 5 _else 6")->equals(new IfExpr(
                new BoolExpr(true),new NumExpr(5), new NumExpr(6))));
        CHECK( parse_str("_if _false _then 5 _else 6")->equals(new IfExpr(
                new BoolExpr(false),new NumExpr(5), new NumExpr(6))));
        CHECK_THROWS_WITH(( parse_str("_if _true then 5 _else 6")), "invalid input");
        CHECK_THROWS_WITH(( parse_str("_if _true _then 5 else 6")), "invalid input");
        CHECK_THROWS_WITH(( parse_str("_if _true _than 5 _else 6")), "invalid input");
        CHECK_THROWS_WITH(( parse_str("_if _true _then 5 _ellse 6")), "invalid input");
        CHECK( parse_str("_let same = 1 == 2 _in  _if 1 == 2 _then _false + 5 _else 88")
                ->interp()->equals(new NumVal(88)));
        CHECK_THROWS_WITH(( parse_str("_if 4 + 1 _then 2 _else 3")->interp()),"Not a boolean");
        CHECK_THROWS_WITH(( parse_str( "(1 == 2) + 3")->interp()), "add of non-number");
        CHECK(parse_str("1==2+3")->interp()->equals(new BoolVal(false)));
        CHECK(parse_str("1+1 == 2+0")->interp()->equals(new BoolVal(true)));
    }
}