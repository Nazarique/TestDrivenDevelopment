#include "../Test.h"

int createTestEntry()
{
    /*
    If this was real code, it might open a
    connection to a database, insert a row
    of date, and return the row identifier.
    */
    return 100;
}

void updateTestEntryName(int /*id*/, std::string_view name)
{
    if (name.empty())
    {
        throw 1;
    }
    /*
    Real code proceed to update the
    data with new name.
    */
}

void deleteTestEntry(int /*id*/)
{
    /*
    Real code would use the id to delete
    the temporary row od date.
    */
}

class TempEntry
{
public:
    void setup() { mId = createTestEntry(); }

    void teardown() { deleteTestEntry(mId); }

    int id() { return mId; }

private:
    int mId;
};

/*
TEST_EX("Test will run setup and teardown code", int)
{
    int id = createTestEntry();
    // If this was a project test, it might be called
    // "Updating empty name throw." And the type thrown
    // Would not be an int.
    updateTestEntryName(id, "");
    deleteTestEntry(id);
}
 */
TEST_EX("Test will run setup and teardown code", int)
{
    TempEntry entry;
    // If this was a project test, it might be called
    // "Updating empty name throw." And the type thrown
    // Would not be an int.
    updateTestEntryName(entry.id(), "");
}