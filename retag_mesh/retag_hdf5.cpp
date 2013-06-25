#include <iostream>
#include <sstream>
#include <iomanip> // for setprecision
#include <limits> // for min/max values
#include <assert.h>
#include <math.h>
#include <time.h>
#include <vector>

#include "MBInterface.hpp"
#include "MBCore.hpp"
#include "MBTagConventions.hpp"
#include "MBTagConventions.hpp"
#include "MBRange.hpp"
#include "moab/Range.hpp"
#include "moab/Core.hpp"
#include "moab/Skinner.hpp"
#include "moab/GeomTopoTool.hpp"

MBInterface *MBI();

struct tag_info {
  moab::Tag mbtag;
  moab::TagType mbtype;
  std::string mbtagname;
};

int main(int argc, char **argv) 
{
  std::string input_name;
  MBErrorCode rval;
  MBEntityHandle input_mesh_set;

  input_name = argv[1]; // set the filename
  rval = MBI()->create_meshset( MESHSET_SET, input_mesh_set ); // create entity set
  if(rval != MB_SUCCESS)
    std::cout << "failed to create meshset" << std::endl;
  rval = MBI()->load_file( input_name.c_str(), &input_mesh_set ); // open the mesh
  if(rval != MB_SUCCESS)
    std::cout << "failed to load file" << std::endl;
  
  // vars to store the list of tag names and 
  std::vector<moab::Tag> tag_list;
  std::vector<moab::Tag>::const_iterator tl_it;
  std::vector<std::string> tag_names;
  std::vector<moab::TagType> tag_types;

  rval = MBI()->tag_get_tags(tag_list); // get the list of tag handles

  for ( tl_it = tag_list.begin() ; tl_it != tag_list.end() ; ++tl_it ) // loop 
    {
      moab::TagType tmp_tagtype;
      moab::Tag mbtag;
      std::string tmp_tagname;

      mbtag = *tl_it; // a given tag handle
      rval = MBI()->tag_get_name(mbtag,tmp_tagname); // get the name of the tag handle
      rval = MBI()->tag_get_type(mbtag,tmp_tagtype); // the type of the tag handle
      tag_types.push_back(tmp_tagtype);
      tag_names.push_back(tmp_tagname);
      std::cout << tmp_tagname << " " << tmp_tagtype << std::endl;
    }

  MBRange dagmc_mat;
  MBTag dagmc_tag;

  rval = MBI()->tag_get_handle("DAGMC_TAG_NAME",1,MB_TYPE_INTEGER,
			       dagmc_tag,moab::MB_TAG_DENSE);
  rval = MBI()->get_entities_by_type_and_tag(input_mesh_set,2,&dagmc_tag,0,0,
					     dagmc_mat);
  /// rval = MBI()->get_entities_by_type_and_tag(0,input_mesh_set,dagmc_mat,NULL,1,dagmc_mat,MBInterface::UNION);
  return 0;
}

MBInterface *MBI() 
{
    static MBCore instance;
    return &instance;
}
