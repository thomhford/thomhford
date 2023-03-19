//
// Created by Thomas Ford on 2/2/23.
//

#include "Expr.h"
#include "catch.h"
#include "Val.h"
#include "parse.h"

TEST_CASE( "William Test Mult" )
{
    SECTION("mult_mults_mults_mults")
    {
        CHECK((new MultExpr( new MultExpr(new NumExpr(10),
                                          new MultExpr(new MultExpr(
                                                  new NumExpr(10),new NumExpr(10)),
                                                       new NumExpr(10))),
                             new MultExpr(new NumExpr(10), new NumExpr(10))))
                             ->to_pretty_string() == "(10 * (10 * 10) * 10) * 10 * 10");
    }
}

TEST_CASE( "Kevin Tests NumExpr Equality" )
{
    NumExpr numValueZero(0);
    NumExpr numValueOne(1);
    NumExpr numValueFive1(5);
    NumExpr numValueFive2(5);
    NumExpr numValueNegativeTen1(-10);
    NumExpr numValueNegativeTen2(-10);
    NumExpr numValueOneMillion1(1000000);
    NumExpr numValueOneMillion2(1000000);
    NumExpr numValueNegOneMillion1(1000000);
    NumExpr numValueNegOneMillion2(1000000);

    SECTION( "Equality with same values" )
    {
        SECTION( "Same positive value" )
        {
            REQUIRE( numValueFive1.equals(&numValueFive2) == true);
            REQUIRE( numValueOneMillion1.equals(&numValueOneMillion2) == true);
        }
        SECTION( "Same negative value" )
        {
            REQUIRE( numValueNegativeTen1.equals(&numValueNegativeTen2) == true);
            REQUIRE( numValueNegOneMillion1.equals(&numValueNegOneMillion2) == true);
        }
        SECTION( "Both values 0" )
        {
            REQUIRE( numValueZero.equals(&numValueZero) == true);
        }
    }
    SECTION( "Equality with different values" )
    {
        SECTION( "Different values with same sign" )
        {
            REQUIRE( numValueOne.equals(&numValueFive1) == false);
            REQUIRE( numValueNegOneMillion1.equals(&numValueNegativeTen1) == false);
        }
        SECTION( "Different values with different sign" )
        {
            REQUIRE( numValueOne.equals(&numValueNegOneMillion1) == false);
            REQUIRE( numValueNegativeTen1.equals( &numValueFive1) == false);
        }
        SECTION( "Non-zero compared to zero" )
        {
            REQUIRE( numValueOne.equals(&numValueZero) == false);
        }
    }
    SECTION( "Equality with expressions that are not type NumExpr" )
    {
        REQUIRE( numValueOne.equals(new AddExpr
                                            (&numValueOne, &numValueZero)) == false);
        REQUIRE( numValueZero.equals(new MultExpr(&numValueOne, &numValueZero)) == false);
        REQUIRE( numValueOneMillion1.equals( new VarExpr( (std::string &)"test" ) ) == false);
    }
}

TEST_CASE( "Kevin Tests AddExpr Equality" )
{
    AddExpr addDoubleZero(new NumExpr(0), new NumExpr(0));
    AddExpr addDoubleZero2(new NumExpr(0), new NumExpr(0));
    AddExpr add11(new NumExpr(1), new NumExpr(1));
    AddExpr addNegativeOnes(new NumExpr(-1), new NumExpr(-1));
    AddExpr addNegativeOnes2(new NumExpr(-1), new NumExpr(-1));
    AddExpr addPosAndNeg(new NumExpr(1000), new NumExpr(-1999));
    SECTION("Equivalent AddExpr Expressions")
    {
        REQUIRE( addDoubleZero.equals(&addDoubleZero2) == true);
        REQUIRE( add11.equals(&add11) == true);
        REQUIRE( addNegativeOnes.equals(&addNegativeOnes2) == true);
    }
    SECTION("Nonequivalent AddExpr Expressions")
    {
        REQUIRE( addDoubleZero.equals(&add11) == false);
        REQUIRE( addPosAndNeg.equals(&addNegativeOnes) == false);
        REQUIRE( add11.equals(&addNegativeOnes2) == false);
    }
    SECTION("Comparison with Non-AddExpr Expressions")
    {
        REQUIRE( add11.equals(new NumExpr
                                      (8)) == false);
        REQUIRE( add11.equals( new MultExpr(new NumExpr
                                                    (0), new NumExpr
                                                    (1))) == false);
        REQUIRE( add11.equals(new VarExpr((std::string &)"test" )) == false);
    }
}

TEST_CASE( "Kevin Tests MultExpr Equality" )
{
    MultExpr multDoubleZero(new NumExpr(0), new NumExpr(0));
    MultExpr multDoubleZero2(new NumExpr(0), new NumExpr(0));
    MultExpr mult11(new NumExpr(1), new NumExpr(1));
    MultExpr multNegativeOnes(new NumExpr(-1), new NumExpr(-1));
    MultExpr multNegativeOnes2(new NumExpr(-1), new NumExpr(-1));
    MultExpr multPosAndNeg(new NumExpr(1000), new NumExpr(-1999));
    SECTION("Equivalent MultExpr Expressions")
    {
        REQUIRE( multDoubleZero.equals(&multDoubleZero2) == true);
        REQUIRE( mult11.equals(&mult11) == true);
        REQUIRE( multNegativeOnes.equals(&multNegativeOnes2) == true);
    }
    SECTION("Nonequivalent MultExpr Expressions")
    {
        REQUIRE( multDoubleZero.equals(&mult11) == false);
        REQUIRE( multPosAndNeg.equals(&multNegativeOnes) == false);
        REQUIRE( mult11.equals(&multNegativeOnes2) == false);
    }
    SECTION("Comparison with Non-MultExpr Expressions")
    {
        REQUIRE( mult11.equals(new NumExpr
                                       (8)) == false);
        REQUIRE( mult11.equals( new AddExpr
                                        (new NumExpr
                                                 (0), new NumExpr
                                                 (1))) == false);
        REQUIRE( mult11.equals(new VarExpr((std::string &)"test" )) == false);
    }
}

TEST_CASE( "Kevin Tests VarExpr Equality" )
{
    std::string emptyString;
    std::string spaceString = " ";
    std::string shortString1 = "dog";
    std::string shortString2 = "cat";
    std::string longString1 = "abcdefghijklmnop";
    std::string longString2 = "This is a looooong string. Holy moly can you believe how long this string is?";
    std::string escapeCharString = "\n\t\"\\\r";
    VarExpr emptyStringVar1(emptyString);
    VarExpr emptyStringVar2(emptyString);
    VarExpr spaceStringVar1(spaceString);
    VarExpr spaceStringVar2(spaceString);
    VarExpr shortStringVar1(shortString1);
    VarExpr shortStringVar2(shortString2);
    VarExpr longStringVar1(longString1);
    VarExpr longStringVar2(longString1);
    VarExpr longStringVar3(longString2);
    VarExpr longStringVar4(longString2);
    VarExpr escapeCharVar1(escapeCharString);
    VarExpr escapeCharVar2(escapeCharString);
    VarExpr var99("test");

    SECTION("Equivalent string variables")
    {
        REQUIRE( emptyStringVar1.equals(&emptyStringVar2) == true);
        REQUIRE( spaceStringVar1.equals(&spaceStringVar2) == true);
        REQUIRE( emptyStringVar1.equals(&emptyStringVar2) == true);
        REQUIRE( shortStringVar1.equals(&shortStringVar1) == true);
        REQUIRE( longStringVar3.equals(&longStringVar4) == true);
        REQUIRE( escapeCharVar1.equals(&escapeCharVar2) == true);
    }

    SECTION("Nonequivalent string variables")
    {
        REQUIRE( emptyStringVar1.equals(&spaceStringVar1) == false);
        REQUIRE( longStringVar1.equals(&longStringVar3) == false);
        REQUIRE( escapeCharVar1.equals(&shortStringVar2) == false);
        REQUIRE( shortStringVar1.equals(&shortStringVar2) == false);
    }

    SECTION("Comparison with non VarExpr Expressions")
    {
        REQUIRE( emptyStringVar1.equals(new NumExpr
                                                (5)) == false);
        REQUIRE( longStringVar1.equals(new AddExpr
                                               (new NumExpr
                                                        (0), new NumExpr
                                                        (1))) == false);
        REQUIRE( shortStringVar1.equals(new MultExpr(new NumExpr
                                                             (0), new AddExpr(new NumExpr
                                                                                      (1), new NumExpr
                                                                                      (9)))) == false);
    }

}

TEST_CASE("Kevin Tests Interpret Tests"){

    SECTION("NumExpr interp() tests"){
        CHECK( (new NumExpr
                (0))->interp() ->equals(new NumVal(0)));
        CHECK( (new NumExpr
                (-1))->interp() ->equals(new NumVal(-1)));
        CHECK( (new NumExpr
                (1))->interp() ->equals(new NumVal(1)));
        CHECK( (new NumExpr
                (123456789))->interp() ->equals(new NumVal(123456789)));
        CHECK( (new NumExpr
                (-66665555))->interp() ->equals(new NumVal(-66665555)));
    }
    SECTION("AddExpr interp() tests"){
        CHECK( (new AddExpr
                (new AddExpr
                         (new NumExpr
                                  (10), new NumExpr
                                  (15)),new AddExpr(new NumExpr
                                                            (20),new NumExpr
                                                            (20))))
                       ->interp()->equals(new NumVal(65)));
        CHECK( (new AddExpr
                (new NumExpr
                         (-99), new NumExpr
                         (99)))->interp() ->equals(new NumVal(0)));
        CHECK( (new AddExpr
                ( new NumExpr
                          (0), new NumExpr
                          (-123456789)))->interp() ->equals(new NumVal(-123456789)));
    }
    SECTION("MultExpr interp() tests"){
        CHECK( (new MultExpr(new NumExpr
                                     (3), new NumExpr
                                     (2)))
                       ->interp()->equals(new NumVal(6)));
        CHECK( (new MultExpr( new AddExpr
                                      (
                                              new NumExpr
                                                      (0),new MultExpr(new NumExpr
                                                                               (99),
                                                                       new NumExpr
                                                                               (2))), new NumExpr
                                      (-1)))->interp() ->equals(new NumVal(-198)));
        CHECK( (new MultExpr(new NumExpr
                                     (12355661), new NumExpr
                                     (0)))->interp() ->equals(new NumVal(0)));
    }
    SECTION("VarExpr interp() tests"){
        CHECK_THROWS_WITH( (new VarExpr("x"))->interp(), "variable expression has no value" );
        CHECK_THROWS_WITH( (new VarExpr("\n"))->interp(), "variable expression has no value" );
    }
}

TEST_CASE("Kevin Tests has_variable() test cases"){
    SECTION("NumExpr has_variable() tests"){
        CHECK_FALSE( (new NumExpr
                (0))->has_variable());
        CHECK_FALSE( (new NumExpr
                (123456789))->has_variable());
        CHECK_FALSE( (new NumExpr
                (-11223344))->has_variable());
    }
    SECTION("AddExpr and MultExpr has_variable() tests"){
        CHECK( (new AddExpr
                (new NumExpr
                         (2), new MultExpr(
                        new AddExpr
                                (new NumExpr
                                         (2), new NumExpr
                                         (99)),
                        new VarExpr("X"))))->has_variable() == true);
        CHECK( (new AddExpr
                (new VarExpr("Y"), new VarExpr("Z")))->has_variable() == true);
        CHECK_FALSE( (new AddExpr
                (new NumExpr
                         (1), new MultExpr(new NumExpr
                                                   (5), new NumExpr
                                                   (45))))->has_variable());
        CHECK_FALSE( (new MultExpr(new NumExpr
                                           (1), new MultExpr(new NumExpr
                                                                     (21), new NumExpr
                                                                     (-64))))->has_variable());
        CHECK_FALSE( (new MultExpr(new NumExpr
                                           (123), new NumExpr
                                           (666)))->has_variable());
        CHECK( (new MultExpr(new VarExpr("Jeepers"), new NumExpr
                (2)))->has_variable() == true);
    }
    SECTION("VarExpr has_variable() tests"){
        CHECK( (new VarExpr("X"))->has_variable() == true);
        CHECK( (new VarExpr("123/\n\"fsdfa"))->has_variable() == true);
        CHECK( (new VarExpr("This one is one more test!"))->has_variable() == true);
    }
}

TEST_CASE("Kevin Tests subst() test cases"){
    SECTION("No substitution tests"){
        CHECK( (new NumExpr
                (6))
                       ->subst("x", new VarExpr("Y"))
                       ->equals(new NumExpr
                                        (6)));
        CHECK( (new AddExpr
                (new NumExpr
                         (1), new NumExpr
                         (2)))
                       ->subst("z", new NumExpr
                               (4))
                       ->equals(new AddExpr
                                        (new NumExpr
                                                 (1), new NumExpr
                                                 (2))));
        CHECK( (new MultExpr(new NumExpr
                                     (-1), new NumExpr
                                     (222)))
                       ->subst("z", new NumExpr
                               (4))
                       ->equals(new MultExpr(new NumExpr
                                                     (-1), new NumExpr
                                                     (222))));
        CHECK( (new VarExpr("notToBeReplaced"))
                       ->subst("X", new VarExpr("shouldn'tBeSubstituted"))
                       ->equals(new VarExpr("notToBeReplaced")));
    }
    SECTION("Tests where substitution occurs"){

        CHECK( (new VarExpr("x"))
                       ->subst("x", new AddExpr
                               (new NumExpr
                                        (1), new NumExpr
                                        (-99)))
                       ->equals(new AddExpr
                                        (new NumExpr
                                                 (1), new NumExpr
                                                 (-99))));
        CHECK( (new AddExpr
                (new VarExpr("x"), new NumExpr
                        (7)))
                       ->subst("x", new VarExpr("y"))
                       ->equals(new AddExpr
                                        (new VarExpr("y"), new NumExpr
                                                (7))) );
        CHECK( (new VarExpr("x"))
                       ->subst("x", new AddExpr
                               (new VarExpr("y"),new NumExpr
                                       (7)))
                       ->equals(new AddExpr
                                        (new VarExpr("y"),new NumExpr
                                                (7))) );
    }
}
TEST_CASE("Kevin Let"){
    SECTION("Interp & Subst tests"){
        auto* letVal6 = new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"),
                                                                     new NumExpr(1)));
        CHECK(letVal6->interp() ->equals(new NumVal(6)));

        auto* letVal8 = new LetExpr("x", new AddExpr(new NumExpr(5), new NumExpr(2)),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK(letVal8->interp() ->equals(new NumVal(8)));

        auto* nestedSameVarLet = new LetExpr("x", new NumExpr(5),
                                             new LetExpr("x",new NumExpr(6),
                                                         new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK(nestedSameVarLet->interp() ->equals(new NumVal(7)));

        auto* nestedSameVarInRhs = new LetExpr("x", new AddExpr(new NumExpr(0), new NumExpr(-12)),
                                                  new LetExpr("x", new AddExpr(new VarExpr("x"),
                                                                               new NumExpr(-13)),
                                                              new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK(nestedSameVarInRhs->interp() ->equals(new NumVal(-24)));
        auto* nestedSameVarInBodyOnly = new LetExpr("x", new AddExpr(new NumExpr(0), new NumExpr(-12)),
                                                       new LetExpr("x", new AddExpr(new NumExpr(13),
                                                                                    new NumExpr(-13)),
                                                                   new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK(nestedSameVarInBodyOnly->interp() ->equals(new NumVal(1)));
        auto* mixedVarNestedLet = new LetExpr("x", new NumExpr(5),
                                              new LetExpr("y",new NumExpr(8),new AddExpr(new
                                              VarExpr("x"), new VarExpr("y"))));
        CHECK(mixedVarNestedLet->interp() ->equals(new NumVal(13)));

        auto* tripleNestedLet = new LetExpr("x", new NumExpr(1),
                                               new LetExpr("y", new NumExpr(1),
                                                           new LetExpr("z", new NumExpr(1),
                                                                       new AddExpr(new AddExpr(new VarExpr("x"),
                                                                                               new VarExpr("y")), new VarExpr("z")))));
        auto* tripleNestedLet2 = new LetExpr("x", new NumExpr(1),
                                                new LetExpr("y", new NumExpr(1),
                                                            new LetExpr("z", new AddExpr
                                                            (new VarExpr("x"), new NumExpr(1)),
                                                            new AddExpr(new AddExpr(new VarExpr("x"),
                                                                                    new VarExpr("y")), new VarExpr("z")))));
        CHECK(tripleNestedLet->interp() ->equals(new NumVal(3)));
        CHECK( (tripleNestedLet2->interp() ->equals(new NumVal(4))));
    }
}
TEST_CASE("John Pretty Print -- AddExpr"){
    auto* testNum1 = new NumExpr(1);
    auto* testNum2 = new NumExpr(2);
    auto* testNum3 = new NumExpr(3);
    auto* testNum4 = new NumExpr(4);

    auto* testVar1 = new VarExpr("a");
    //auto* testVar2 = new VarExpr("b");

    auto* testAdd1 = new AddExpr(testNum1, testNum2);
    auto* testAdd2 = new AddExpr(testNum3, testAdd1);
    auto* testAdd3 = new AddExpr(testAdd1, testNum3);
    auto* testAdd4 = new AddExpr(testNum3, testNum4);
    auto* testAdd5 = new AddExpr(testAdd1, testAdd4);

    auto* testAdd6 = new AddExpr(testNum1, testVar1);

    auto* testMult1 = new MultExpr(testNum3, testNum4);

    auto* testMixed1 = new AddExpr(testNum1, testMult1);

    std::stringstream out("");
    testAdd2->pretty_print(out);
    CHECK(out.str() == "3 + 1 + 2");

    out.str("");
    testAdd3->pretty_print(out);
    CHECK(out.str() == "(1 + 2) + 3");

    out.str("");
    testMixed1->pretty_print(out);
    CHECK(out.str() == "1 + 3 * 4");

    out.str("");
    testAdd5->pretty_print(out);
    CHECK(out.str() == "(1 + 2) + 3 + 4");

    out.str("");
    testAdd6->pretty_print(out);
    CHECK(out.str() == "1 + a");
}


TEST_CASE("John Pretty Print -- MultExpr"){
    auto* testNum1 = new NumExpr(1);
    auto* testNum2 = new NumExpr(2);
    auto* testNum3 = new NumExpr(3);
    auto* testNum4 = new NumExpr(4);

    auto* testAdd1 = new AddExpr(testNum1, testNum2);
    auto* testAdd2 = new AddExpr(testNum3, testNum4);

    auto* testMult1 = new MultExpr(testNum1, testNum2);
    auto* testMult2 = new MultExpr(testNum3, testMult1);
    auto* testMult3 = new MultExpr(testMult1, testNum3);
    auto* testMult4 = new MultExpr(testNum3, testNum4);
    auto* testMult5 = new MultExpr(testMult1, testMult4);

    std::stringstream out("");
    testMult2->pretty_print(out);
    CHECK(out.str() == "3 * 1 * 2");

    out.str("");
    testMult3->pretty_print(out);
    CHECK(out.str() == "(1 * 2) * 3");

    out.str("");
    testMult5->pretty_print(out);
    CHECK(out.str() == "(1 * 2) * 3 * 4");

    auto* testMixed1 = new MultExpr(testNum3, testAdd1);
    auto* testMixed2 = new MultExpr(testAdd1, testNum3);
    auto* testMixed3 = new MultExpr(testAdd1, testAdd2);

    out.str("");
    testMixed1->pretty_print(out);
    CHECK(out.str() == "3 * (1 + 2)");

    out.str("");
    testMixed2->pretty_print(out);
    CHECK(out.str() == "(1 + 2) * 3");

    out.str("");
    testMixed3->pretty_print(out);
    CHECK(out.str() == "(1 + 2) * (3 + 4)");
}
TEST_CASE("John's Subst - _let"){
    auto *varX = new VarExpr("x");
    auto *varY = new VarExpr("y");
    auto *num1 = new NumExpr(1);
    auto *num5 = new NumExpr(5);
    auto *add1 = new AddExpr(varX, num1);
    auto *add2 = new AddExpr(varY, num1);
    auto *add3 = new AddExpr(varX, num5);
    auto *add4 = new AddExpr(varX, add1);
    auto *add5 = new AddExpr(varY, add1);
    auto *let1 = new LetExpr("x", num1, add1);
    auto *let2 = new LetExpr("x", num1, add2);
    auto *let3 = new LetExpr("x", varY, add3);
    auto *let4 = new LetExpr("x", add4, add1);
    auto *let5 = new LetExpr("x", add5, add1);


    // x = 1
    std::stringstream out;
    let1->subst("x", num1)->print(out);     // Add1 = x+1
    CHECK(out.str() == "(_let x=1 _in (x+1))");

    out.str("");
    let2->subst("x", num1)->print(out);     // Add2 = y+1
    CHECK(out.str() == "(_let x=1 _in (y+1))");

    out.str("");
    let3->subst("x", num1)->print(out);     // Add3 = x+5
    CHECK(out.str() == "(_let x=y _in (x+5))");

    out.str("");
    let4->subst("x", num1)->print(out);
    CHECK(out.str() == "(_let x=(1+(1+1)) _in (x+1))");

    out.str("");
    let5->subst("x", num1)->print(out);
    CHECK(out.str() == "(_let x=(y+(1+1)) _in (x+1))");

    // y = 1
    out.str("");
    let1->subst("y", num1)->print(out);     // Add1 = x+1
    CHECK(out.str() == "(_let x=1 _in (x+1))");

    out.str("");
    let2->subst("y", num1)->print(out);     // Add2 = y+1
    CHECK(out.str() == "(_let x=1 _in (1+1))");

    out.str("");
    let3->subst("y", num1)->print(out);     // Add3 = x+5
    CHECK(out.str() == "(_let x=1 _in (x+5))");

    out.str("");
    let4->subst("y", num1)->print(out);
    CHECK(out.str() == "(_let x=(x+(x+1)) _in (x+1))");

    out.str("");
    let5->subst("y", num1)->print(out);
    CHECK(out.str() == "(_let x=(1+(x+1)) _in (x+1))");

    // x = x
    out.str("");
    let1->subst("x", varX)->print(out);     // Add1 = x+1
    CHECK(out.str() == "(_let x=1 _in (x+1))");

    out.str("");
    let2->subst("x", varX)->print(out);     // Add2 = y+1
    CHECK(out.str() == "(_let x=1 _in (y+1))");

    out.str("");
    let3->subst("x", varX)->print(out);     // Add3 = x+5
    CHECK(out.str() == "(_let x=y _in (x+5))");

    out.str("");
    let4->subst("x", varX)->print(out);
    CHECK(out.str() == "(_let x=(x+(x+1)) _in (x+1))");

    out.str("");
    let5->subst("x", varX)->print(out);
    CHECK(out.str() == "(_let x=(y+(x+1)) _in (x+1))");

    // x = y
    out.str("");
    let1->subst("y", varX)->print(out);     // Add1 = x+1
    CHECK(out.str() == "(_let x=1 _in (x+1))");

    out.str("");
    let2->subst("y", varX)->print(out);     // Add2 = y+1
    CHECK(out.str() == "(_let x=1 _in (x+1))");

    out.str("");
    let3->subst("y", varX)->print(out);     // Add3 = x+5
    CHECK(out.str() == "(_let x=x _in (x+5))");

    out.str("");
    let4->subst("y", varX)->print(out);
    CHECK(out.str() == "(_let x=(x+(x+1)) _in (x+1))");

    out.str("");
    let5->subst("y", varX)->print(out);
    CHECK(out.str() == "(_let x=(x+(x+1)) _in (x+1))");
}
TEST_CASE("John's Pretty Print Matthew's examples"){           //We created these tests from your (Matthew's) examples.
    std::stringstream out("");
    (new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))->pretty_print(out);
    CHECK(out.str() == "_let x = 5\n"
                       "_in  x + 1");
    out.str(std::string());
    (new AddExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new NumExpr(1)))->pretty_print(out);
    CHECK(out.str() == "(_let x = 5\n"
                       " _in  x) + 1");
    out.str(std::string());
    (new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)))))->pretty_print(out);
    CHECK(out.str() == "5 * _let x = 5\n"
                       "    _in  x + 1");
    out.str(std::string());
    (new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new VarExpr("x"))), new NumExpr(1)))->pretty_print(out);
    CHECK(out.str() == "5 * (_let x = 5\n"
                       "     _in  x) + 1");
    out.str(std::string());
    (new LetExpr("x", new NumExpr(5), new AddExpr(new LetExpr("y", new NumExpr(3), new AddExpr(new VarExpr("y"), new NumExpr(2))), new VarExpr("x"))))
            ->pretty_print(out);
    CHECK(out.str() == "_let x = 5\n"
                       "_in  (_let y = 3\n"
                       "      _in  y + 2) + x");
    out.str(std::string());
    (new LetExpr("x", new LetExpr("y", new NumExpr(6), new MultExpr(new VarExpr("y"), new NumExpr(2))), new AddExpr(new VarExpr("x"), new NumExpr(1))))->pretty_print(out);
    CHECK(out.str() == "_let x = _let y = 6\n"
                       "         _in  y * 2\n"
                       "_in  x + 1");

}
TEST_CASE("Pretty Print examples_Kevin"){           //Created from assignment examples
    std::stringstream out("");
    (new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))->pretty_print(out);
    CHECK(out.str() == "_let x = 5\n"
                       "_in  x + 1");
    out.str(std::string());
    (new AddExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new NumExpr(1)))->pretty_print(out);
    CHECK(out.str() == "(_let x = 5\n"
                       " _in  x) + 1");
    out.str(std::string());
    (new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)))))->pretty_print(out);
    CHECK(out.str() == "5 * _let x = 5\n"
                       "    _in  x + 1");
    out.str(std::string());
    (new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new VarExpr("x"))), new NumExpr(1)))->pretty_print(out);
    CHECK(out.str() == "5 * (_let x = 5\n"
                       "     _in  x) + 1");
    out.str(std::string());
    (new LetExpr("x", new NumExpr(5), new AddExpr(new LetExpr("y", new NumExpr(3), new AddExpr(new VarExpr("y"), new NumExpr(2))), new VarExpr("x"))))
            ->pretty_print(out);

    CHECK(out.str() == "_let x = 5\n"
                       "_in  (_let y = 3\n"
                       "      _in  y + 2) + x");
    out.str(std::string());
    (new LetExpr("x", new LetExpr("y", new NumExpr(6), new MultExpr(new VarExpr("y"), new NumExpr(2))), new AddExpr(new VarExpr("x"), new NumExpr(1))))->pretty_print(out);
    CHECK(out.str() == "_let x = _let y = 6\n"
                       "         _in  y * 2\n"
                       "_in  x + 1");

}

TEST_CASE("pretty_print_let_mine_some_reuse_of_kevin_triple_nested_let") {
    auto* tripleNestedLet = new LetExpr("x", new NumExpr(1),
                                           new LetExpr("y", new NumExpr(1),
                                                       new MultExpr(new AddExpr(new VarExpr("x"), new VarExpr("y")), new VarExpr("z"))));
    auto* tripleNestedLet2 = new LetExpr("x", new NumExpr(1),
                                            new LetExpr("y", new NumExpr(1),
                                                        new LetExpr("z", new AddExpr(new VarExpr("x"), new NumExpr(1)),
                                                                    new AddExpr(new AddExpr(new VarExpr("x"), new VarExpr("y")), new VarExpr("z")))));
    auto* tripleNestedLet3 = new LetExpr("x", new NumExpr(1),
                                            new LetExpr("y", new NumExpr(1),
                                                        new LetExpr("z", new AddExpr(new VarExpr("x"), new NumExpr(1)),
                                                                    new MultExpr(new AddExpr(new VarExpr("x"), new VarExpr("y")), new VarExpr("z")))));


    CHECK(tripleNestedLet -> to_pretty_string() ==
          "_let x = 1\n"
          "_in  _let y = 1\n"
          "     _in  (x + y) * z"
    );
    CHECK(tripleNestedLet2 -> to_pretty_string() ==
          "_let x = 1\n"
          "_in  _let y = 1\n"
          "     _in  _let z = x + 1\n"
          "          _in  (x + y) + z"
    );
    CHECK(tripleNestedLet3 -> to_pretty_string() ==
          "_let x = 1\n"
          "_in  _let y = 1\n"
          "     _in  _let z = x + 1\n"
          "          _in  (x + y) * z"
    );
    auto* tripleNestedLet4 =new LetExpr("x", new NumExpr(5),
                                           new LetExpr("y", new NumExpr(3),
                                                       new AddExpr(new VarExpr("y"), new NumExpr(2))));
    auto* tripleNestedLet5 =new LetExpr("x", new NumExpr(5),
                                           new AddExpr(new LetExpr("y", new NumExpr(3),
                                                                   new AddExpr(new VarExpr("y"), new NumExpr(2))), new VarExpr("x")));
    std::stringstream out("");
    out.str("");
    tripleNestedLet4->pretty_print(out);
    CHECK(out.str() ==
          "_let x = 5\n"
          "_in  _let y = 3\n"
          "     _in  y + 2"
    );

    CHECK(tripleNestedLet5 -> to_pretty_string() == "_let x = 5\n"
                                                          "_in  (_let y = 3\n"
                                                          "      _in  y + 2) + x");
    SECTION("assignment_examples") {
        CHECK( (new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new VarExpr("x"))), new NumExpr(1)))-> to_pretty_string()
               == "5 * (_let x = 5\n"
                  "     _in  x) + 1");
//        CHECK( (new MultExpr(new MultExpr(new NumExpr (2), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x") ,new  NumExpr(1)) )), new NumExpr(3) )) -> to_pretty_string()
//               == "(2 * _let x = 5\n"
//                  "     _in  x + 1) * 3");
    }
        // A _let needs parentheses when it is nested immediately as the right argument of an unparenthesized *
        // where _let would have needed parentheses in the surrounding context
        // (that is, if the _let used in place of the whole * would need parentheses,
        // then it still needs parentheses within the right-hand size of *).
    SECTION("new_edge") {
        CHECK( (new MultExpr(new NumExpr (2), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x") ,new  NumExpr(1)) ) )) -> to_pretty_string()
               == "2 * _let x = 5\n"
                  "    _in  x + 1");
        CHECK( (new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new MultExpr(new VarExpr("x"), new NumExpr(2)))), new NumExpr(1)))-> to_pretty_string()
               == "5 * (_let x = 5\n"
                  "     _in  x * 2) + 1");
        CHECK( (new MultExpr((new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new MultExpr(new VarExpr("x"), new NumExpr(2)))), new NumExpr(1))), new NumExpr(7)))-> to_pretty_string()
               == "(5 * (_let x = 5\n"
                  "      _in  x * 2) + 1) * 7");
        CHECK( (new LetExpr("x", new NumExpr(10), new MultExpr( new MultExpr(new VarExpr("x"), new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)), new NumExpr(10))), new MultExpr(new NumExpr(10), new NumExpr(10)))))
                       ->to_pretty_string()  == "_let x = 10\n"
                                                      "_in  (x * (10 * 10) * 10) * 10 * 10");
        CHECK( (new LetExpr("x", new NumExpr(1), new MultExpr( new MultExpr(new VarExpr("x"), new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)), new VarExpr("x"))), new MultExpr(new NumExpr(10), new NumExpr(10)))))
                       -> to_pretty_string() == "_let x = 1\n"
                                                      "_in  (x * (10 * 10) * x) * 10 * 10" );
        CHECK( (new LetExpr("x", new NumExpr(1), new MultExpr( new MultExpr(new VarExpr("x"), new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)), new VarExpr("x"))), new MultExpr(new VarExpr("y"), new NumExpr(10)))))
                       -> to_pretty_string() == "_let x = 1\n"
                                                      "_in  (x * (10 * 10) * x) * y * 10" );
    }
}

TEST_CASE("Let_equals_mine") {
    SECTION("Values_same") {
        REQUIRE( (new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) ))->equals(new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) )));
    }
    SECTION("Values_same_different_rhs") {
        REQUIRE( !(new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) ))->equals(new LetExpr("x", new NumExpr(5), new AddExpr(new NumExpr(2), new VarExpr("x")) )));
    }
    SECTION("Values_same_different_lhs") {
        REQUIRE( !(new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) ))->equals(new LetExpr("y", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) )));
    }
    SECTION("Values_same_different_body") {
        REQUIRE( !(new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) ))->equals(new LetExpr("x", new NumExpr(4), new MultExpr(new NumExpr(3), new VarExpr("y")) )));
    }
    SECTION("different_types") {
        REQUIRE( !(new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) ))->equals( new MultExpr(new NumExpr(3), new VarExpr("y")) ));
    }
}
TEST_CASE("Let_has_variable_mine") {
    SECTION("has") {
        REQUIRE( (new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new VarExpr("x")) ))->has_variable());
    }
    SECTION("does_not_has") {
        REQUIRE( !(new LetExpr("x", new NumExpr(4), new AddExpr(new NumExpr(2), new NumExpr(4)) ))->has_variable());
    }
}
TEST_CASE("Let_print_mine") {
    CHECK( (new LetExpr("x", new NumExpr(5), new AddExpr(new LetExpr("y", new NumExpr(3), new AddExpr(new VarExpr("y"), new NumExpr(2))), new VarExpr("x")))) -> to_string()
           == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");
    CHECK( (new LetExpr("x", new NumExpr(1),
                        new LetExpr("y", new NumExpr(1),
                                    new LetExpr("z", new AddExpr(new VarExpr("x"), new NumExpr(1)),
                                                new MultExpr(new AddExpr(new VarExpr("x"), new VarExpr("y")), new VarExpr("z")))))) -> to_string()
           == "(_let x=1 _in (_let y=1 _in (_let z=(x+1) _in ((x+y)*z))))");
}
TEST_CASE ("Let_interp_mine") {
    SECTION("hw_examples") {
        CHECK((new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new VarExpr("x"))), new NumExpr(1))) -> interp() ->equals(new NumVal(26)));
        CHECK((new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))) -> interp() ->equals(new NumVal(30)));
    }
    SECTION("from_pretty_print_edge") {
        CHECK( (new LetExpr("x", new NumExpr(1),
                            new LetExpr("y", new NumExpr(1),
                                        new LetExpr("z", new AddExpr(new VarExpr("x"), new NumExpr(1)),
                                                    new MultExpr(new AddExpr(new VarExpr("x"), new VarExpr("y")), new VarExpr("z")))))) -> interp() ->equals(new NumVal(4)));
        CHECK( (new MultExpr((new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new MultExpr(new VarExpr("x"), new NumExpr(2)))), new NumExpr(1))), new NumExpr(7))) -> interp() ->equals(new NumVal(357))); // 51 * 7
        CHECK( (new LetExpr("x", new NumExpr(10), new MultExpr( new MultExpr(new VarExpr("x"), new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)), new NumExpr(10))), new MultExpr(new NumExpr(10), new NumExpr(10)))))
                       ->interp()  ->equals(new NumVal(1000000)));
        CHECK( (new LetExpr("x", new NumExpr(1), new MultExpr( new MultExpr(new VarExpr("x"), new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)), new VarExpr("x"))), new MultExpr(new NumExpr(10), new NumExpr(10)))))
                       ->interp()  ->equals(new NumVal(10000)));
        CHECK_THROWS_WITH( ((new LetExpr("x", new NumExpr(1), new MultExpr( new MultExpr(new VarExpr("x"), new MultExpr(new MultExpr(new NumExpr(10), new NumExpr(10)), new VarExpr("x"))), new MultExpr(new VarExpr("y"), new NumExpr(10)))))
                                    -> interp() ->equals(new NumVal(10000))), "variable expression has no value");
    }
    SECTION("bypass_middle_let") {
        CHECK ((new LetExpr("x", new NumExpr(2), new LetExpr("z", new NumExpr(4), new AddExpr(new VarExpr("x"), new NumExpr(10)))))
                       -> interp() ->equals(new NumVal(12)));
    }
}
TEST_CASE ("Parse boolean/conditionals from William") {
    SECTION("from quiz") {
        CHECK((parse_str("_if 1 == 2 _then 5 _else 6"))->
                equals(new IfExpr(new EqExpr(new NumExpr(1), new NumExpr(2)), new NumExpr(5), new NumExpr(6))));

        CHECK((((parse_str("_if 1 == 2 _then 5 _else 6"))->interp())->to_string()) == "6");
        CHECK((((parse_str("1 == 2"))->interp())->to_string()) == "_false");
        CHECK((((parse_str("(1 + 2) == (3 + 0)"))->interp())->to_string()) == "_true");
        CHECK((((parse_str("1 + 2 == 3 + 0"))->interp())->to_string()) == "_true");
        CHECK_THROWS_WITH(((((parse_str("(1 == 2) + 3 "))->interp())->to_string()) == "_true"), "add of non-number");
        CHECK((((parse_str("1==2+3"))->interp())->to_string()) == "_false");
        CHECK((((parse_str("_if _false\n"
                           "_then 5\n"
                           "_else 6"))->interp())->to_string()) == "6");
        CHECK((((parse_str("_if _false\n"
                           "_then _false\n"
                           "_else _true"))->interp())->to_string()) == "_true");
        CHECK((((parse_str("_if _false\n"
                           "_then 5\n"
                           "_else _false"))->interp())->to_string()) == "_false");
        CHECK_THROWS_WITH(((((parse_str("_true + _false"))->interp())->to_string()) == "_false"), "add of non-number");
        CHECK_THROWS_WITH(((((parse_str("_true + 1"))->interp())->to_string()) == "_false"), "add of non-number");
        CHECK((((parse_str("_true == _true"))->interp())->to_string()) == "_true");
        CHECK((((parse_str("1 == _true"))->interp())->to_string()) == "_false");
        CHECK_THROWS_WITH(((((parse_str("_if 1 + 2\n"
                                        "_then _false\n"
                                        "_else _true"))->interp())->to_string()) == "_false"),
                          "Not a boolean");
        CHECK((((parse_str("_if _true\n"
                           "_then 5\n"
                           "_else _true + 1"))->interp())->to_string()) == "5");
        CHECK_THROWS_WITH(((((parse_str("_if _false\n"
                                        "_then 5\n"
                                        "_else _true + 1"))->interp())->to_string()) == "_false"), "add of non-number");
        CHECK_THROWS_WITH(((((parse_str("_let x = _true + 1\n"
                                        "_in  _if _true\n"
                                        "     _then 5\n"
                                        "     _else x"))->interp())->to_string()) == "_false"), "add of non-number");
        CHECK_THROWS_WITH(((((parse_str("_let x = _true + 1\n"
                                        "_in  _if _true\n"
                                        "     _then 5\n"
                                        "     _else x"))->interp())->to_string()) == "_false"), "add of non-number");
        CHECK((((parse_str("(_if _true\n"
                           " _then 5\n"
                           " _else _true) + 1"))->interp())->to_string()) == "6");
        CHECK((((parse_str("_if (_if 1 == 2\n"
                           "     _then _false\n"
                           "     _else _true)\n"
                           "_then 5\n"
                           "_else 6"))->interp())->to_string()) == "5");
        CHECK((((parse_str("_if (_if 1 == 2\n"
                           "     _then _true\n"
                           "      _else _false)\n"
                           "_then 5\n"
                           "_else 6"))->interp())->to_string()) == "6");
    }
}