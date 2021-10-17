#include "WirePositionTimingDisplay.hh"


WirePositionTimingDisplay::WirePositionTimingDisplay(const std::string F, const std::string T):
  Display(F,T),
  fAPA(-1){

  for (size_t i=0; i<12; ++i) {
    int low = 1600 + 2560*i;
    int high = low + 960;
    Boundary<int> b(low,high);
    fAPA_Bounds[i] = b;
  }
}


void WirePositionTimingDisplay::LookForAPA(const GenType gen) {

  std::vector<size_t> type_count;
  std::cout << "Gen requested " << gen << std::endl; 
  for (size_t it=0; it < im.Hit_True_GenType->size(); ++it) {
    if ((*im.Hit_True_GenType)[it] == gen) {
      type_count.push_back(it);
    }
  }
  // if (1000.*(*im.True_ENu)[0] < 10 ||
  //     1000.*(*im.True_ENu)[0] > 11) {
  //   fAPA = -1;
  //   return;
  // }
  
  if (type_count.size() == 0) {
    std::cout << "Didn't find this event in the whole detector trying next event." << std::endl;
    // fAPA = -1;
    // return;
  } else if (type_count.size() == 1) {
    fChan = (*im.Hit_Chan)[type_count[0]];
    fTime = (*im.Hit_Time)[type_count[0]];
  } else {
    size_t it = type_count[type_count.size()/2];
    fChan = (*im.Hit_Chan)[it];
    fTime = (*im.Hit_Time)[it];
  }
  for (auto const& apa : fAPA_Bounds) {
    if (apa.second.IsInOrOnBorder(fChan)) {
      fAPA = apa.first;
      break;
    }
  }
  // std::cout << "Energy " << 1000.*(*im.True_ENu)[0] << " MeV"<< std::endl;
  // std::cout << "X " << (*im.True_VertX)[0] << " cm"<< std::endl;
}

void WirePositionTimingDisplay::DisplayEvent(const int nevent=-1, const int gentype=-1) {

  GenType gen = kOther;
  if (gentype==-1) {
    gen = kSNnu;
  } else {
    gen = ConvertIntToGenType(gentype);
  }
  int ev = std::max(0,nevent);
  while (fAPA == -1) {
    std::cout << "Considering event " << ev << std::endl;
    if (ev>=im.GetEntries()){
      std::cout << "Event requested is too large, or I didn't find an event in the file that you provided." << std::endl;
      exit(1);
    }
    im.GetEntry(ev);
    LookForAPA(gen);
    ev++;
  }
  //int spread = 50;
  int spread = 200;
  f_map_wire_time = Get2DHistos("EventDisplay",";Channel number;Time [ticks];ADC",
                                2*spread,fChan-spread,fChan+spread,
                                2*spread,fTime-spread,fTime+spread);

  std::map<GenType, size_t> nHit_type;
  for (size_t i=0; i<im.Hit_True_GenType->size(); ++i) {
    int chan = (*im.Hit_Chan)[i];
    if (fAPA_Bounds[fAPA].IsInOrOnBorder(chan)) {
      GenType g = ConvertIntToGenType((*im.Hit_True_GenType)[i]);
      nHit_type[g]++;
      float initial_time = (*im.Hit_Time)[i];
      //float adc = (*im.Hit_SADC)[i];
      //f_map_wire_time[g]->Fill(chan,initial_time,adc*2);
      f_map_wire_time[g]->Fill(chan,initial_time);
      std::cout << g << " -> " << chan << " : " << (*im.Hit_RMS)[i] << std::endl;
      
      for(size_t time=0; time<(*im.Hit_RMS)[i];++time) {
        //f_map_wire_time[g]   ->Fill(chan,initial_time-(*im.Hit_RMS)[i]/2+time,adc);
        //f_map_wire_time[kAll]->Fill(chan,initial_time-(*im.Hit_RMS)[i]/2+time,adc);
        //f_map_wire_time[g]   ->Fill(chan,initial_time-(*im.Hit_RMS)[i]/2+time);
        //f_map_wire_time[kAll]->Fill(chan,initial_time-(*im.Hit_RMS)[i]/2+time);
        f_map_wire_time[g]   ->Fill(chan,initial_time+time);
        f_map_wire_time[kAll]->Fill(chan,initial_time+time);
      }
    }
    
  }
  c->Print("WireTimingDisplay.pdf[");
  gPad->SetRightMargin(1.5*gPad->GetRightMargin());
  gPad->SetGridx();
  gPad->SetGridy();
  gStyle->SetPalette(1);
  gStyle->SetOptStat("orme");
  for (auto const& it: f_map_wire_time) {
    it.second->SetEntries(nHit_type[it.first]);
    it.second->SetStats(1);
    //it.second->SetMaximum(500);
    it.second->SetMinimum(0);
    it.second->Draw("COLZ");
    c->Print("WireTimingDisplay.pdf");
  }
  c->Print("WireTimingDisplay.pdf]");
}
